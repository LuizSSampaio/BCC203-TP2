// Declara BufferedTape, uma abstração de fita que usa um buffer do tamanho de uma página para reduzir operações de I/O e registra os comprimentos dos runs contíguos para permitir sua leitura e intercalação.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "Common.hpp"
#include "Item.hpp"
#include "Tape.hpp"

namespace FMais1 {

// Fita com buffer de página e controle de blocos (runs) contíguos.
class BufferedTape {
  public:
    BufferedTape();

    void Clear();
    void WriteRecord(const Item &item);
    void EndRun();
    void Flush();
    void Rewind();
    bool ReadRecord(Item &item);

    bool HasNextRun() const;
    uint64_t BeginNextRun();
    std::size_t NumRuns() const;
    void SetRecordMetrics(bool enabled);

  private:
    std::unique_ptr<Tape> tape_;
    std::array<Item, PAGE_SIZE> buffer_{};
    std::size_t bufPos_ = 0;
    std::vector<uint64_t> runs_;
    uint64_t currentRunLength_ = 0;
    uint64_t totalRecords_ = 0;
    uint64_t recordsRead_ = 0;
    std::size_t nextRun_ = 0;
    bool reading_ = false;
};

}  // namespace FMais1
