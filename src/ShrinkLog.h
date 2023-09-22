//
// Created by rbanasza on 18.09.2023.
//

#ifndef SHRINKLOG_SHRINKLOG_H
#define SHRINKLOG_SHRINKLOG_H

#include <filesystem>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

//TODO: Remove this
/*
 * Control flow:
 * - ProcessProject gets path to project
 * - It iterates over every file in the project and calls ProcessFile (use many workers!)
 * - For every file:
 *      - find all print macros and generate hashes
 *      - generate new hash block
 *      - find and replace existing hash block
 *      - save existing hash block to MasterMap
 *      - make it thread safe!
 *      (proposition: store MasterMap in memory and prepare
 *      a thread safe interface to append new hashes)
 */

namespace sl {
    namespace stdf = std::filesystem;

    class ShrinkLog final {
        using stringRange = std::pair<decltype(begin(std::string{})), decltype(end(std::string{}))>;
        enum class InternalStatus {
            OK = 0,
            UnableToOpenFile,
            UnableToFindPrintArguments,
        };

        static std::unordered_set<std::string> fileExtensions;
        static std::string logMacroName;
        static std::regex pattern;
        static unsigned int minimumStringWidthToCheck;

        ShrinkLog::InternalStatus ProcessFile(const stdf::path& pth) const noexcept;
        [[nodiscard]] std::vector<std::string> FindPrintMacros(const std::string&) const noexcept;
        [[nodiscard]] std::vector<stringRange> FindUncommentedRanges(const std::string &fileContent, unsigned minimumRangeWidth = 1) const noexcept;
        [[nodiscard]] std::vector<std::string> GetHashes(const std::vector<std::string>&) const noexcept;
        [[nodiscard]] std::string ReplaceFileHashBlock(std::string_view) const noexcept;

    public:
        enum class OutputStatus {
            OK = 0,
            WrongProjectPath,
            EmptyProjectDirectory
        };
        [[nodiscard]] OutputStatus ProcessProject(const stdf::path&, const unsigned threadCount = 1) noexcept;


    };

} // sl

#endif //SHRINKLOG_SHRINKLOG_H
