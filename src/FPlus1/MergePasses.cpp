// Implementa as operações das passadas de intercalação: seleciona repetidamente o menor registro entre os runs de entrada usando o heap, grava os runs resultantes na fita de saída e os redistribui entre as fitas de entrada para a próxima passada.
#include "MergeFPlus1.hpp"

#include <array>
#include <stdexcept>

#include "Item.hpp"

void BalancedMergeFPlus1::ReadOrThrow(FMais1::BufferedTape &tape, Item &item) {
    if (!tape.ReadRecord(item)) {
        throw std::runtime_error(
            "Fim inesperado de fita durante a intercalação");
    }
}

bool BalancedMergeFPlus1::AnyInputHasRun() const {
    for (std::size_t t = 0; t < kInputTapes; ++t) {
        if (this->tapes_[t]->HasNextRun()) {
            return true;
        }
    }
    return false;
}

// Intercala uma rodada das fitas de entrada na fita de saída.
void BalancedMergeFPlus1::MergePass() {
    FMais1::BufferedTape &out = *this->tapes_[kInputTapes];
    out.Clear();
    for (std::size_t t = 0; t < kInputTapes; ++t) {
        this->tapes_[t]->Rewind();
    }

    // Registros restantes no bloco atual de cada fita.
    std::array<uint64_t, kInputTapes> remaining{};

    while (this->AnyInputHasRun()) {
        this->heap_.Clear();

        // Carrega o primeiro registro de cada bloco da rodada.
        for (std::size_t t = 0; t < kInputTapes; ++t) {
            FMais1::BufferedTape &in = *this->tapes_[t];
            if (!in.HasNextRun()) {
                continue;
            }

            remaining[t] = in.BeginNextRun();
            FMais1::HeapEntry &slot = this->heap_.Append();
            slot.tape = static_cast<uint32_t>(t);
            ReadOrThrow(in, slot.item);
            --remaining[t];
        }
        this->heap_.Build();

        // Grava o menor registro e repõe pela mesma fita.
        while (!this->heap_.Empty()) {
            FMais1::HeapEntry &root = this->heap_.Root();
            out.WriteRecord(root.item);

            const uint32_t t = root.tape;
            if (remaining[t] > 0) {
                ReadOrThrow(*this->tapes_[t], root.item);
                --remaining[t];
                this->heap_.FixRoot();
            } else {
                this->heap_.PopRoot();
            }
        }
        out.EndRun();
    }

    out.Flush();

    // As fitas de entrada foram totalmente consumidas.
    for (std::size_t t = 0; t < kInputTapes; ++t) {
        this->tapes_[t]->Clear();
    }
}

// Redistribui os blocos de saída entre as fitas de entrada.
void BalancedMergeFPlus1::Redistribute() {
    FMais1::BufferedTape &out = *this->tapes_[kInputTapes];
    out.Rewind();

    Item record{};
    const std::size_t runCount = out.NumRuns();
    for (std::size_t j = 0; j < runCount; ++j) {
        const uint64_t length = out.BeginNextRun();
        FMais1::BufferedTape &target = *this->tapes_[j % kInputTapes];

        for (uint64_t k = 0; k < length; ++k) {
            ReadOrThrow(out, record);
            target.WriteRecord(record);
        }
        target.EndRun();
    }

    for (std::size_t t = 0; t < kInputTapes; ++t) {
        this->tapes_[t]->Flush();
    }
}
