// Implementa a geração inicial de runs por seleção por substituição: lê os registros do arquivo em páginas, mantém até 19 registros no heap, distribui os runs ordenados pelas fitas de entrada e contabiliza comparações e valida a quantidade lida.
#include "MergeFPlus1.hpp"

#include <array>
#include <stdexcept>
#include <string>

#include "Common.hpp"
#include "Item.hpp"

// Gera blocos ordenados por seleção por substituição.
void BalancedMergeFPlus1::GenerateRuns() {
    struct MetricsScope {
        File &file;
        ~MetricsScope() { file.SetRecordMetrics(false); }
    };
    this->file_->SetRecordMetrics(true);
    const MetricsScope metricsGuard{*this->file_};

    this->file_->Rewind();

    std::array<Item, PAGE_SIZE> page{};
    uint64_t pageIdx = 0;
    uint64_t pageCount = 0;
    uint64_t consumed = 0;

    auto readInput = [&](Item &item) {
        if (consumed >= this->file_->quantity()) {
            return false;
        }
        if (pageIdx >= pageCount) {
            if (this->file_->eof()) {
                return false;
            }
            page = this->file_->GetNextPage();
            pageCount = this->file_->lastPageCount();
            pageIdx = 0;
            if (pageCount == 0) {
                return false;
            }
        }
        item = page[pageIdx++];
        ++consumed;
        return true;
    };

    // Carrega a memória inicial.
    this->heap_.Clear();
    while (!this->heap_.Full()) {
        FMais1::HeapEntry &slot = this->heap_.Append();
        if (!readInput(slot.item)) {
            this->heap_.DropLast();
            break;
        }
    }
    if (this->heap_.Empty()) {
        throw std::runtime_error("O arquivo de entrada não possui registros");
    }
    this->heap_.Build();

    std::size_t tapeIndex = 0;  // fita que recebe o bloco corrente
    uint32_t currentRun = 0;    // número do bloco corrente
    uint64_t runCount = 1;

    while (!this->heap_.Empty()) {
        FMais1::HeapEntry &root = this->heap_.Root();

        // Ao mudar de bloco, avança para a próxima fita.
        if (root.run != currentRun) {
            this->tapes_[tapeIndex]->EndRun();
            tapeIndex = (tapeIndex + 1) % kInputTapes;
            currentRun = root.run;
            ++runCount;
        }

        this->tapes_[tapeIndex]->WriteRecord(root.item);

        const float lastKey = root.item.grade;
        const uint32_t lastRun = root.run;

        if (readInput(root.item)) {
            // Uma comparação de chaves: o novo registro ainda cabe no bloco?
            Metrics::RecordKeyComparison();
            root.run = (root.item.grade < lastKey) ? lastRun + 1 : lastRun;
            this->heap_.FixRoot();
        } else {
            this->heap_.PopRoot();
        }
    }
    this->tapes_[tapeIndex]->EndRun();
    for (std::size_t t = 0; t < kInputTapes; ++t) {
        this->tapes_[t]->Flush();
    }
    this->runs_ = runCount;

    if (consumed < this->file_->quantity()) {
        throw std::runtime_error(
            "O arquivo de entrada possui apenas " + std::to_string(consumed) +
            " registros, mas foram pedidos " +
            std::to_string(this->file_->quantity()));
    }
}
