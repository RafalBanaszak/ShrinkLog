//
// Created by rbanasza on 18.09.2023.
//

#ifndef SHRINKLOG_PROJECTPROCESSOR_H
#define SHRINKLOG_PROJECTPROCESSOR_H

#include <filesystem>
#include <regex>
#include <string>
#include <span>
#include <shared_mutex>
#include <thread>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>

#include <xxhash.h>

#include "ArgToBytesCount.h"

//TODO: Remove this
/*
 * Control flow:
 * - ProcessProject gets path to project
 * - It iterates over every file in the project and calls ProcessFile (use many workers!)
 * - For every file:
 *      - find all print macros and generate hashes
 *      - generate new defineName block
 *      - find and replace existing defineName block
 *      - save existing defineName block to MasterMap
 *      - make it thread safe!
 *      (proposition: store MasterMap in memory and prepare
 *      a thread safe interface to append new hashes)
 */

namespace sl {
    namespace stdf = std::filesystem;
    using string_span = std::span<std::string::value_type>;

    class ProjectProcessor final {
        enum class InternalStatus {
            OK = 0,
            UnableToOpenFile,
            UnableToFindPrintArguments,
            MasterHashMapCollisionDetected,
        };

        struct LogMacro {
            string_span tag;           ///< View to file content. Points a tag - STAG_xxxxxxx
            std::string message;           ///< View to file content. Points a message - "..."
            std::string defineName;         ///< String which represents new STAG string
            std::string defineValueArgs;    ///< String which represents value of the STAG #define macro
            unsigned id;
            std::vector<unsigned> argumentsSizes;
        };

        static std::unordered_set<std::string> fileExtensions;
        static std::string logMacroName;
        static std::regex logPattern;
        static std::regex argPattern;
        static unsigned int minimumStringWidthToCheck;

        std::unordered_map<std::string, LogMacro> masterHashMap;
        std::shared_mutex mhmMutex;
        ArgToBytesCount argToBytesCountConverter;

        [[nodiscard]] InternalStatus ProcessFile(const stdf::path& pth) noexcept;
        void AppendToMasterHashMap(const std::vector<LogMacro> &logs) noexcept;
        void GeneratePartialDefineValues(std::vector<LogMacro>& logs) const noexcept;
        void GenerateDefineNames(std::vector<LogMacro>& logs) const noexcept;
        [[nodiscard]] std::vector<LogMacro> FindPrintMacros(const std::string &fileContent) const noexcept;
        [[nodiscard]] std::vector<string_span> FindUncommentedParts(std::string &fileContent, unsigned minimumRangeWidth = 1) const noexcept;

        [[nodiscard]] std::string ReplaceFileHashBlock(std::string_view) const noexcept;
    public:
        enum class OutputStatus {
            OK = 0,
            WrongProjectPath,
            EmptyProjectDirectory
        };

        [[nodiscard]] OutputStatus ProcessProject(const stdf::path&, const unsigned threadCount = 1) noexcept;

        void ReplaceTags(std::vector<LogMacro> logs) const noexcept;
    };

} // sl

#endif //SHRINKLOG_PROJECTPROCESSOR_H
