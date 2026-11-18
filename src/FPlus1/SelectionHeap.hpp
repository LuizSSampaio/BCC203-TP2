// Declara um heap mínimo de capacidade fixa para a seleção por substituição e a intercalação: cada entrada associa um registro ao número do run e à fita de origem, e o heap ordena primeiro pelo run e depois pela nota, registrando as comparações de chaves.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>

#include "Common.hpp"
#include "Item.hpp"

namespace FMais1 {

// Registro armazenado no heap e seus metadados de bloco/fita.
struct HeapEntry {
    Item item;
    uint32_t run = 0; // O heap dá prioridade ao run atual; dentro dele, escolhe a menor nota.
    uint32_t tape = 0; // durante a intercalação, indica de qual fita veio o registro. Assim, quando ele sai do heap, o programa sabe em qual fita buscar o próximo registro.
};

// Heap de mínimo com capacidade fixa, ordenado por bloco e nota.
template <std::size_t Capacity>
class SelectionHeap {
  public:
    SelectionHeap() = default;

    bool Empty() const { return size_ == 0; }
    bool Full() const { return size_ == Capacity; }
    void Clear() { size_ = 0; }

    // Reserva uma posição para inserir um registro.
    HeapEntry &Append() {
        if (size_ == Capacity) {
            throw std::logic_error("Heap cheio: limite de memória interna");
        }
        data_[size_].run = 0;
        data_[size_].tape = 0;
        return data_[size_++];
    }

    // Remove a posição reservada se a leitura falhar.
    void DropLast() {
        if (size_ > 0) {
            --size_;
        }
    }

    // Constrói o heap.
    void Build() {
        for (std::size_t i = size_ / 2; i > 0; --i) {
            SiftDown(i - 1);
        }
    }

    // Acesso à raiz; chamar FixRoot() após alterá-la.
    HeapEntry &Root() { return data_[0]; }
    void FixRoot() { SiftDown(0); }

    // Remove a raiz.
    void PopRoot() {
        --size_;
        if (size_ > 0) {
            data_[0] = data_[size_];
            SiftDown(0);
        }
    }

  private:

    // a deve sair antes de b?
    bool Less(const HeapEntry &a, const HeapEntry &b) {
        if (a.run != b.run) {
            return a.run < b.run;
        }
        Metrics::RecordKeyComparison();
        return a.item.grade < b.item.grade;
    }

    // Reorganiza o heap depois que um registro muda de posição ou valor
    void SiftDown(std::size_t i) {
        while (true) {
            const std::size_t left = 2 * i + 1;
            if (left >= size_) {
                return;
            }

            std::size_t smallest = left;
            if (left + 1 < size_ && Less(data_[left + 1], data_[left])) {
                smallest = left + 1;
            }
            if (!Less(data_[smallest], data_[i])) {
                return;
            }

            std::swap(data_[i], data_[smallest]);
            i = smallest;
        }
    }

    std::array<HeapEntry, Capacity> data_{};
    std::size_t size_ = 0;
};

}  // namespace FMais1
