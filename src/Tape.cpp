#include "Tape.hpp"
#include "Common.hpp"
#include <cstdint>
#include <filesystem>
#include <format>

Tape::Tape() {
    this->id_ = Tape::lastId_++;

    this->file_ =
        std::fstream(this->GetPath(), std::ios::in | std::ios::out |
                                          std::ios::trunc | std::ios::binary);

    if (!this->file_.is_open()) {
        throw std::runtime_error("Failed to open tape file");
    }
}

Tape::~Tape() {
    this->file_.close();

    std::filesystem::remove(this->GetPath());
}

// TODO: Add checks for uncompleted blocks
std::array<Item, PAGE_SIZE> Tape::ReadBlock() {
    this->file_.seekg(sizeof(std::array<Item, PAGE_SIZE>) * this->pos_);

    std::array<Item, PAGE_SIZE> block;

    this->file_.read(reinterpret_cast<char *>(&block),
                     sizeof(std::array<Item, PAGE_SIZE>));
    this->AdvanceBlock();

    return block;
}

void Tape::WriteBlock(std::array<Item, PAGE_SIZE> block) {
    this->file_.seekg(sizeof(std::array<Item, PAGE_SIZE>) * this->pos_);

    this->file_.write(reinterpret_cast<char *>(&block),
                      sizeof(std::array<Item, PAGE_SIZE>));
    this->AdvanceBlock();
}

void Tape::AdvanceBlock() { this->pos_++; }

void Tape::RetreatBlock() {
    if (this->pos_ <= 0) {
        return;
    }

    this->pos_--;
}

uint32_t Tape::GetId() const { return this->id_; }

uint32_t Tape::GetCurrentPosition() const { return this->pos_; };

std::string Tape::GetPath() const {
    return std::format("./tmp/tape_{}", this->id_);
}
