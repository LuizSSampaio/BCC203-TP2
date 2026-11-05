#include "File.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>

#include "Common.hpp"
#include "Item.hpp"

File::File(const std::string &path, uint64_t quantity)
    : path_(path), quantity_(quantity) {
    this->file_ = std::ifstream(path);

    if (!this->file_.is_open()) {
        throw std::runtime_error("Failed to open file");
    }
}

File::~File() { this->file_.close(); }

std::array<Item, PAGE_SIZE> File::GetNextPage() {
    std::array<Item, PAGE_SIZE> page{};

    if (this->eof()) {
        Log::Error("Invalid input file access index");
        return page;
    }

    constexpr size_t REG_LEN = 8;
    constexpr size_t GRADE_LEN = 5;
    constexpr size_t STATE_LEN = 2;
    constexpr size_t CITY_LEN = 50;
    constexpr size_t DEGREE_LEN = 30;

    uint64_t counter = 0;
    std::string line;
    while (counter < 100 && std::getline(this->file_, line)) {
        if (line.size() <
            (REG_LEN + GRADE_LEN + STATE_LEN + CITY_LEN + DEGREE_LEN)) {
            continue;
        }

        Item reg = Item();
        size_t offset = 0;

        reg.registrationNumber = std::stol(line.substr(offset, REG_LEN));
        offset += REG_LEN;

        reg.grade = std::stof(line.substr(offset, GRADE_LEN));
        offset += GRADE_LEN;

        line.copy(reg.state.data(), STATE_LEN, offset);
        offset += STATE_LEN;

        line.copy(reg.city.data(), CITY_LEN, offset);
        offset += CITY_LEN;

        line.copy(reg.degree.data(), DEGREE_LEN, offset);

        page[counter] = reg;

        counter++;
    }

    return page;
}

std::string File::path() const { return this->path_; }

std::filesystem::file_time_type File::lastModification() const {
    return std::filesystem::last_write_time(this->path_);
}

uint64_t File::size() const { return std::filesystem::file_size(this->path_); }

uint64_t File::quantity() const { return this->quantity_; }

bool File::eof() {
    if (this->file_.eof()) {
        return true;
    }

    auto const pos = this->file_.tellg();
    if (pos < 0) {
        return true;
    }

    return (static_cast<uint64_t>(pos) / sizeof(Item)) >= this->quantity_;
}
