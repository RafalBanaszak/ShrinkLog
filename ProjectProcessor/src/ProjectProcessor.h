// MIT License
//
// Copyright (c) 2024 ShrinkLog Contributors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef SHRINKLOG_PROJECTPROCESSOR_H
#define SHRINKLOG_PROJECTPROCESSOR_H

#include <filesystem>
#include <optional>
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

#include "ArgEncoder.h"

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
            std::vector<ArgEncoder::Argument> argSzs; ///< List of argument sizes in unsigned format
        };

        static std::unordered_set<std::string> fileExtensions;
        static std::string logMacroName;
        static std::regex logPattern;
        static std::regex argPattern;
        static unsigned int minimumStringWidthToCheck;

        stdf::path projectPath;
        unsigned maxId;
        std::unordered_map<std::string, LogFunction> masterHashMap;
        std::mutex printMutex;
        std::shared_mutex mhmMutex;
        std::optional<ArgEncoder> argEncoderOpt;

        [[nodiscard]] InternalStatus ProcessFile(const stdf::path& pth) noexcept;
        [[nodiscard]] static std::vector<LogFunction> FindPrints(const std::string &fileContent) noexcept;
        [[nodiscard]] static std::vector<StringSpan> FilterUncommentedParts(std::string &fileContent, unsigned minimumRangeWidth = 1) noexcept;
        [[nodiscard]] static std::string SimplifyMultilineString(std::string &&str) noexcept;
        void ExtractArguments(std::vector<LogFunction>& logs) const noexcept;
        static void GenerateTagNames(std::vector<LogFunction>& logs) noexcept;
        void AppendToMasterHashMap(const std::vector<LogFunction> &logs) noexcept;
        static void ReplaceTags(std::vector<LogFunction> &logs) noexcept;
        void AssignIDs() noexcept;
        void UnifyArgumentTags() noexcept;
        [[nodiscard]] InternalStatus GenerateMapFile(stdf::path pth) const noexcept;
        [[nodiscard]] InternalStatus GenerateHeaderFile(stdf::path pth) const noexcept;

    public:
        enum class OutputStatus {
            WrongProjectPath = -100,
            InvalidConfiguration,
            EmptyProjectDirectory,
            OK = 0
        };

        [[nodiscard]] OutputStatus ProcessProject(const stdf::path &projectPth, const uint8_t threadCount = 1) noexcept;
    };

} // sl

#endif //SHRINKLOG_PROJECTPROCESSOR_H
