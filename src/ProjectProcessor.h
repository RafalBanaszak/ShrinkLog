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

namespace sl {
    namespace stdf = std::filesystem;
    // use a span instead the string_view. The SV doesn't allow to modify content it points to
    using StringSpan = std::span<std::string::value_type>;

    class ProjectProcessor final {
        enum class InternalStatus {
            OK = 0,
            UnableToOpenFile,
            UnableToFindPrintArguments,
            FileGenerationError
        };

        struct LogFunction {
            StringSpan stagView;          ///< View to a specific part of a string which holds file content. Points a stagView - SLOG_xxxxxxx
            std::string message;          ///< Contains a log message
            std::string stagNewName;      ///< New STAG name e.g. SLOG_7aYZVYc
            std::string stagEncArgs;      ///< Encoded arguments list e.g. "\0x01\0x04..."
            unsigned stagId;              ///< Identifies LOG message, a natural number
            std::vector<unsigned> argSzs; ///< List of argument sizes in integer format
        };

        static std::unordered_set<std::string> fileExtensions;
        static std::string logMacroName;
        static std::regex logPattern;
        static std::regex argPattern;
        static unsigned int minimumStringWidthToCheck;

        unsigned maxId;
        std::unordered_map<std::string, LogFunction> masterHashMap;
        std::shared_mutex mhmMutex;
        ArgToBytesCount argToBytesCountConverter;

        [[nodiscard]] InternalStatus ProcessFile(const stdf::path& pth) noexcept;
        [[nodiscard]] std::vector<LogFunction> FindPrints(const std::string &fileContent) const noexcept;
        [[nodiscard]] std::vector<StringSpan> FilterUncommentedParts(std::string &fileContent, unsigned minimumRangeWidth = 1) const noexcept;
        void ExtractArguments(std::vector<LogFunction>& logs) const noexcept;
        void GenerateTagNames(std::vector<LogFunction>& logs) const noexcept;
        void AppendToMasterHashMap(const std::vector<LogFunction> &logs) noexcept;
        void ReplaceTags(std::vector<LogFunction> logs) const noexcept;
        void AssignIDs() noexcept;
        [[nodiscard]] InternalStatus GenerateMapFile(stdf::path pth) const noexcept;
        [[nodiscard]] InternalStatus GenerateHeaderFile(stdf::path pth) const noexcept;

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
