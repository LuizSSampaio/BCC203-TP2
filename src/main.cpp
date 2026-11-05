#include <filesystem>
#include <memory>

#include "Common.hpp"
#include "File.hpp"

enum FileType : uint8_t {
    Ascending = 1,
    Descending = 2,
    Unordered = 3,
};

int main(int argc, char *argv[]) {
    constexpr int requiredArgC = 4;
    if (argc < requiredArgC) {
        Log::Error("Use ordena <method> <quantity> <situation> [-P]");
        return -1;
    }

    constexpr int optionalLastArgPos = 4;
    if (argc == (optionalLastArgPos + 1) &&
        std::string(argv[optionalLastArgPos]) == std::string("-P")) {
        Log::enableInfo = true;
    }

    long int const quantity = std::atol(argv[2]);
    constexpr int maxFileSize = 471705;
    if (quantity > maxFileSize || quantity <= 0) {
        Log::Error("<quantity> must be a value grater than 0 and equal or "
                   "less than 2000000");
        return -1;
    }

    int const situationValue = std::atoi(argv[3]);
    if (situationValue < FileType::Ascending ||
        situationValue > FileType::Unordered) {
        Log::Error(
            "<situation> must be one of these: 1 -> ascending file; 2 -> "
            "descending file; 3 -> unordered file");
        return -1;
    }

    std::string fileTypeName;
    switch (static_cast<FileType>(situationValue)) {
    case Ascending:
        fileTypeName = "provao_ascending";
        break;
    case Descending:
        fileTypeName = "provao_descending";
        break;
    case Unordered:
        fileTypeName = "provao_shuffled";
        break;
    }

    // std::filesystem is used because the old implementation had an issue with
    // the location of the executable
    // The generator create the file at ./tmp but the old code searched at
    // ./build/tmp
    // I've update it to handle these two cases.
    std::filesystem::path filePath = "tmp/" + fileTypeName;
    if (!std::filesystem::exists(filePath)) {
        const auto execPath = std::filesystem::path(argv[0]);
        const auto candidateParent =
            execPath.parent_path() / ".." / "tmp" / fileTypeName;
        const auto candidateSame =
            execPath.parent_path() / "tmp" / fileTypeName;

        if (std::filesystem::exists(candidateParent)) {
            filePath = candidateParent;
        } else if (std::filesystem::exists(candidateSame)) {
            filePath = candidateSame;
        }
    }
    std::shared_ptr<File> const file =
        std::make_shared<File>(filePath.string(), quantity);

    Log::Error("Error: <method>, must be one of these: 1 -> indexed "
               "sequencial access; 2 -> binary tree; 3 -> B tree; 4 -> B* "
               "tree");
    return -1;
}
