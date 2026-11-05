#pragma once

#include "Common.hpp"
#include "Item.hpp"
#include <array>
#include <cstdint>
#include <fstream>

class Tape {
  public:
    Tape();
    ~Tape();

    std::array<Item, PAGE_SIZE> ReadBlock();
    void WriteBlock(std::array<Item, PAGE_SIZE> block);

    void AdvanceBlock();
    void RetreatBlock();

    uint32_t GetId() const;
    uint32_t GetCurrentPosition() const;

  private:
    static uint32_t lastId_;

    uint32_t id_;
    uint32_t pos_ = 0;
    std::fstream file_;

    std::string GetPath() const;
};
