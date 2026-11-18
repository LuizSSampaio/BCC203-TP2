// Implementa a inicialização e o fluxo principal do método de intercalação balanceada f+1: gera runs, executa passadas de intercalação até obter um único run, grava o resultado no arquivo binário e fornece a função que integra o método ao executor de ordenações.
#include "MergeFPlus1.hpp"

#include <array>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "BinaryIO.hpp"
#include "Common.hpp"
#include "MethodRunner.hpp"

BalancedMergeFPlus1::BalancedMergeFPlus1(std::shared_ptr<File> file)
    : file_(std::move(file)) {
    if (this->file_ == nullptr) {
        throw std::invalid_argument("Arquivo de entrada inválido");
    }

    for (std::size_t i = 0; i < kTotalTapes; ++i) {
        this->tapes_.push_back(std::make_unique<FMais1::BufferedTape>());
    }
}

void BalancedMergeFPlus1::Sort() {
    Metrics::Reset();
    Metrics::SetPhase(Phase::Preprocessing);

    const auto p1Start = std::chrono::steady_clock::now();
    this->GenerateRuns();
    const auto p1End = std::chrono::steady_clock::now();
    this->phase1TimeMs_ =
        std::chrono::duration<double, std::milli>(p1End - p1Start).count();

    Log::Info("Blocos gerados pela seleção por substituição: " +
              std::to_string(this->runs_));

    Metrics::SetPhase(Phase::Search);
    const auto p2Start = std::chrono::steady_clock::now();
    if (this->runs_ <= 1) {
        // Um único bloco já está ordenado.
        this->result_ = this->tapes_[0].get();
    } else {
        while (true) {
            this->MergePass();
            ++this->mergePasses_;

            FMais1::BufferedTape &out = *this->tapes_[kInputTapes];
            Log::Info("Passo " + std::to_string(this->mergePasses_) +
                      " de intercalação: " + std::to_string(out.NumRuns()) +
                      " bloco(s) na fita de saída");

            if (out.NumRuns() == 1) {
                this->result_ = &out;
                break;
            }
            this->Redistribute();
        }
    }
    const auto p2End = std::chrono::steady_clock::now();
    this->phase2TimeMs_ =
        std::chrono::duration<double, std::milli>(p2End - p2Start).count();

    this->WriteResultToBinary();
}

void BalancedMergeFPlus1::WriteResultToBinary() {
    if (this->result_ == nullptr) {
        return;
    }

    this->result_->SetRecordMetrics(false);
    this->result_->Rewind();

    this->file_->CloseStream();
    std::ofstream out(this->file_->binaryPath(),
                      std::ios::binary | std::ios::trunc);
    if (!out.is_open()) {
        throw std::runtime_error(
            "Falha ao abrir arquivo binário intermediário para escrita: " +
            this->file_->binaryPath());
    }

    const std::string writeError =
        "Falha ao gravar resultado ordenado no arquivo binário";

    std::array<Item, PAGE_SIZE> buffer{};
    std::size_t bufPos = 0;
    Item item{};

    while (this->result_->ReadRecord(item)) {
        buffer[bufPos++] = item;
        if (bufPos == PAGE_SIZE) {
            BinaryIO::WriteItemsPage(out, buffer, bufPos, writeError);
            bufPos = 0;
        }
    }

    BinaryIO::WriteItemsPage(out, buffer, bufPos, writeError);

    out.flush();
    if (!out) {
        throw std::runtime_error(
            "Falha ao finalizar gravação do arquivo binário intermediário");
    }
    out.close();

    this->result_->SetRecordMetrics(true);
    this->file_->ReopenStream();
}

bool BalancedMergeFPlus1::CheckAndShowResult(bool print) {
    if (this->result_ == nullptr) {
        return false;
    }
    return this->file_->VerifySorted(print);
}

int RunMethod2(const std::shared_ptr<File> &file) {
    return RunSortMethod(file, [](const std::shared_ptr<File> &inputFile) {
        BalancedMergeFPlus1 sorter(inputFile);
        sorter.Sort();

        SortRunResult result;
        result.phase1TimeMs = sorter.phase1TimeMs();
        result.phase2TimeMs = sorter.phase2TimeMs();
        result.valid = sorter.runs() > 0;
        result.report.title =
            "=== Método 2: intercalação balanceada f+1 fitas "
            "(seleção por substituição) ===";
        result.report.extraMetrics = {
            {"Blocos gerados:                    ", sorter.runs()},
            {"Passos de intercalação:            ", sorter.mergePasses()},
        };
        result.report.readLabel = "Transferências de leitura (blocos):";
        result.report.writeLabel = "Transferências de escrita (blocos):";
        return result;
    });
}
