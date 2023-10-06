//
// Created by rbanasza on 18.09.2023.
//

#ifndef SHRINKLOG_PROJECTPROCESSOR_H
#define SHRINKLOG_PROJECTPROCESSOR_H

#include <filesystem>
#include <regex>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>

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

    class ProjectProcessor final {
        enum class InternalStatus {
            OK = 0,
            UnableToOpenFile,
            UnableToFindPrintArguments,
            MasterHashMapCollisionDetected,
        };

        struct LogMacro {
            std::string_view log;           ///< View to file content. Points a whole log function - LOG(...);
            std::string_view tag;           ///< View to file content. Points a tag - STAG_xxxxxxx
            std::string_view message;       ///< View to file content. Points a message - "..."
            XXH64_hash_t hash;              ///< Hash calculated from message string
            std::string defineName;         ///< String which represents new STAG string
            std::string defineValue;        ///< String which represents value of the STAG #define macro
        };

        static std::unordered_set<std::string> fileExtensions;
        static std::string logMacroName;
        static std::regex logPattern;
        static std::regex argPattern;
        static unsigned int minimumStringWidthToCheck;

        std::unordered_map<XXH64_hash_t, std::string> masterHashMap;
        std::mutex mhmMutex;
        ArgToBytesCount argToBytesCountConverter;

        [[nodiscard]] InternalStatus ProcessFile(const stdf::path& pth) noexcept;
        void AppendToMasterHashMap(const std::vector<LogMacro> &logs) noexcept;
        void GenerateDefineValue(std::vector<LogMacro>& logs) const noexcept;
        void GenerateHashes(std::vector<LogMacro>& logs) const noexcept;
        [[nodiscard]] std::vector<LogMacro> FindPrintMacros(const std::string &fileContent) const noexcept;
        [[nodiscard]] std::vector<std::string_view> FindUncommentedParts(const std::string &fileContent, unsigned minimumRangeWidth = 1) const noexcept;

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

#endif //SHRINKLOG_PROJECTPROCESSOR_H
