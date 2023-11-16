//
// Created by rbanasza on 18.09.2023.
//

#include "ProjectProcessor.h"

#include <atomic>
#include <algorithm>
#include <chrono>
#include <deque>
#include <fstream>
#include <ranges>
#include <sstream>

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <hs/hs.h>
#include <xxhash.h>

#include "ThreadPool.h"

namespace sl {
    std::unordered_set<std::string> ProjectProcessor::fileExtensions = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp",
                                                                        ".hxx"};
    //LOG\((SLOG_[a-zA-Z0-9_]{7})  -- log function and stagView
    //[\s\\\n]*,[\s\\\n]           -- comma with 0-n space/backslash/newline before and/or after
    //("(?:[^"\\]|\\.)*"(?:\s*"(?:[^"\\]|\\.)*")*)\s*\)\s*;  -- C/C++ string which can be concatenated finished by );
    std::regex ProjectProcessor::logPattern{R"###(LOG\((SLOG_[a-zA-Z0-9_]{7})[\s\\\n]*,[\s\\\n]*("(?:[^"\\]|\\.)*"(?:\s*"(?:[^"\\]|\\.)*")*)\s*\)\s*;)###"};
    std::regex ProjectProcessor::argPattern{R"###(%[-+ #0]*[\d]*(?:\.\d*)?(hh|h|l|ll|j|z|t|L)?([csdioxXufFeEaAgGnp]))###"};

    unsigned ProjectProcessor::minimumStringWidthToCheck{17};

    ProjectProcessor::InternalStatus ProjectProcessor::ProcessFile(const stdf::path &pth) noexcept {
        //TODO: Replace the file loading method to mmap style (faster)
        using std::fstream;
        fstream fileStream{pth, std::ios::in | std::ios::out};
        if (not fileStream.is_open()) {
            fmt::print(stderr, "Unable to open the file: {}\n", absolute(pth).string());
            return InternalStatus::UnableToOpenFile;
        }

        std::ostringstream fileContentOSS{};
        fileContentOSS << fileStream.rdbuf();
        auto fileContentStr = fileContentOSS.str();

        auto printArguments = FindPrints(fileContentStr); //get stagView and message
        if (printArguments.empty()) {
            fmt::print(stderr, "Error during extracting print macros in the file: {} \n", absolute(pth).string());
            return InternalStatus::UnableToFindPrintArguments;
        }

        ExtractArguments(printArguments);
        GenerateTagNames(printArguments);
        AppendToMasterHashMap(printArguments);
        ReplaceTags(printArguments);

        try{
            fileStream.seekg(0, std::ios::beg);
            fileStream << fileContentStr;
            fileStream.close();
        } catch (const std::exception& e) {
            fmt::print(stderr, "Unable to save the file.\n{}\n{}", pth.string(), e.what());
        }

        return ProjectProcessor::InternalStatus::OK;
    }

    std::vector<ProjectProcessor::LogFunction> ProjectProcessor::FindPrints(const std::string &fileContent) noexcept {
        const auto fileParts = FilterUncommentedParts(const_cast<std::string &>(fileContent), minimumStringWidthToCheck);

        //TODO: Replace std::regex by https://github.com/intel/hyperscan
        std::vector<LogFunction> result;
        result.reserve(100);
        std::smatch match;
        ssize_t bufferShift = 0;
        for (auto & part: fileParts) {
            //TODO: convert span to string only for std::regex. Remove after switch to hyperscan
            std::string content{part.begin(), part.end()};
            bufferShift = 0;
            while (regex_search(content, match, logPattern)) {
                auto logStart       = part.begin() + bufferShift + match.position();
                auto logEnd         = logStart + match.length();
                auto tagStart       = part.begin() + bufferShift + match.position(1);
                auto tagEnd         = tagStart + match.length(1);
                auto message            = SimplifyMultilineString(match.str(2));

                result.push_back({{tagStart, tagEnd}, message});
                bufferShift = std::distance(part.begin(), logEnd);
                content = match.suffix();
            }
        }
        return result;
    }

    std::vector<StringSpan>
    ProjectProcessor::FilterUncommentedParts(std::string &fileContent, unsigned minimumRangeWidth) noexcept {
        std::vector<StringSpan> result{};

        if (fileContent.length() < minimumRangeWidth) {
            return result;
        }

        // helper iterators to mark begin and end of the uncommented string range
        // initially assume the whole file didn't have any parts
        std::string::iterator partBegin, partEnd;
        partBegin = fileContent.begin();
        partEnd = fileContent.end();

        auto it = fileContent.begin();
        bool firstSlashDetected{}, inComment{}, blockComment{}, endStarDetected{};

        // Helper function
        // Add second element to range only if the range will be valid and meet minimum width requirements
        const auto SetRangeIfValid = [&result, &partBegin, &partEnd, minimumRangeWidth](auto it) {
            auto rangeEnd = it;
            if (std::distance(partBegin, rangeEnd) >= minimumRangeWidth) {
                partEnd = rangeEnd;
                result.emplace_back(partBegin, partEnd);
            }
        };

        while (it != fileContent.end()) {
            //READ THIS CODE BOTTOM UP

            //level 3: match block comment last character
            if (endStarDetected) {
                if (*it == '/'){
                    //end of block comment
                    firstSlashDetected = false;
                    inComment = false;
                    blockComment = false;
                    endStarDetected = false;
                    partBegin = it + 1;
                } else {
                    endStarDetected = false;
                }
            }
            //level 2: match single line comment end or first character of block comment end
            else if (inComment) {
                if (blockComment) {
                    if (*it == '*') {
                        endStarDetected = true;
                    }
                } else if (*it == '\n') {
                    //end of single line comment
                    firstSlashDetected = false;
                    inComment = false;
                    partBegin = it + 1;
                }
            }
                // level 1: match second comment start character (/ or *)
            else if (firstSlashDetected) {
                if (*it == '*') {
                    inComment = true;
                    blockComment = true;
                    SetRangeIfValid(it);
                } else if (*it == '/') {
                    inComment = true;
                    SetRangeIfValid(it - 2);
                } else { firstSlashDetected = false; }
            }
                // level 0: check if character is a slash
            else if (*it == '/') {
                firstSlashDetected = true;
            }

            ++it;
        }

        // Uncommented file ranges are recognized as the text between comments.
        // The beginning of an uncommented range is either the beginning of the file or the end of the last comment.
        // The end of an uncommented range is the start of the next comment.
        //
        // In a file without any comments, the end of the first uncommented section would never be detected
        // (since there's no start of the next comment).
        //
        // In a file ended by a code the end of the last uncommented section would never be detected
        // (since there's no start of the next comment).
        //
        // The code below covers both issues.
        if (!inComment || result.empty()) {
            SetRangeIfValid(it);
        }

        return result;
    }

    std::string ProjectProcessor::SimplifyMultilineString(const std::string& str) noexcept {
        static const std::regex re("\"(.*?)\""); // Regular expression to match quoted substrings
        std::smatch match;
        std::string result;
        auto begin = str.cbegin();

        while (std::regex_search(begin, str.cend(), match, re)) {
            result += match[1].str(); // Add the matched substring (without quotes) to the result
            begin = match.suffix().first;
        }

        return result;
    }

    void ProjectProcessor::ExtractArguments(std::vector<LogFunction>& logs) const noexcept {
        //TODO: Replace std::regex by https://github.com/intel/hyperscan
        std::smatch match;
        for (auto& log : logs) {
            std::string content{log.message};
            std::string buffer;
            buffer.reserve(48);

            while (regex_search(content, match, argPattern)) {
                auto byteCnt = argToBytesCountConverter.value().GetByteSize(match[2], match[1]);
                log.argSzs.push_back(byteCnt);
                buffer.append(fmt::format("\\x{:02x}", byteCnt));

                content = match.suffix();
            }
            log.stagEncArgs = std::move(buffer);
        }
    }

    void ProjectProcessor::GenerateTagNames(std::vector<LogFunction> &logs) noexcept {
        //helper function to convert to base63 (a-z, A-Z, 0-9, _)
        auto encodeBase63 = [](XXH64_hash_t input) -> std::string {
            constexpr unsigned digitNumber = 7;
            std::string result;
            result.reserve(digitNumber);
            for (int i = 0; i < digitNumber; ++i) {
                char symbol = static_cast<char>(input % 63);
                input /= 63;
                if (symbol < 10) {
                    symbol += 48;
                } else if (symbol < 10 + 26) {
                    symbol += 65 - 10;
                } else if (symbol < 10 + 26 + 26) {
                    symbol += 97 - (10 + 26);
                } else {
                    symbol = '_';
                }
                result.push_back(symbol);
            }
            return result;
        };

        for (auto &log : logs){
            auto hash = XXH3_64bits(log.message.data(), log.message.length());
            log.stagNewName = "SLOG_" + encodeBase63(hash);
        }
    }

    void ProjectProcessor::AppendToMasterHashMap(const std::vector<LogFunction> &logs) noexcept {
        std::lock_guard guard(mhmMutex);

        for (auto const& log : logs) {
            masterHashMap[log.message] = log;
        }
    }

    void ProjectProcessor::ReplaceTags(std::vector<LogFunction> &logs) noexcept {
        for (auto& log : logs) {
            std::copy(log.stagNewName.cbegin(), log.stagNewName.cend(), log.stagView.begin());
        }
    }

    void ProjectProcessor::AssignIDs() noexcept {
        unsigned id = 0;
        for (auto& mapEntry : masterHashMap) {
            mapEntry.second.stagId = ++id;
        }
        maxId = id;
    }

    ProjectProcessor::InternalStatus ProjectProcessor::GenerateMapFile(stdf::path pth) const noexcept {
        try {
            pth /= "slog";
            stdf::create_directory(pth);
            std::ofstream fileHandler(pth / "map.slog", std::ios::trunc | std::ios::out);

            for (const auto& mapEntry : masterHashMap) {
                auto log = mapEntry.second;
                fileHandler << fmt::format("{};{};{}\n", log.stagId, fmt::join(log.argSzs, " "), log.message);
            }

        } catch (const std::exception& e) {
            fmt::print(stderr, "Map file generating error\n{}", e.what());
            return InternalStatus::FileGenerationError;
        }

        return InternalStatus::OK;
    }

    ProjectProcessor::InternalStatus ProjectProcessor::GenerateHeaderFile(stdf::path pth) const noexcept {
        try {
            pth /= "slog";
            stdf::create_directory(pth);
            std::ofstream fileHandler(pth / "tags.h", std::ios::trunc | std::ios::out);

            unsigned byteCnt;
            if (maxId < 0xFFU) {
                byteCnt = 1;
            } else if (maxId < 0xFFFFU) {
                byteCnt = 2;
            } else {
                byteCnt = 3;
            }

            fileHandler << fmt::format("#define SLOG_ID_BYTE_CNT {}\n", byteCnt);
            fileHandler << "#define SLOG_INT_B_SIZE 4\n";
            fileHandler << "/* #define SLOG_ENABLE_8B */\n";
            fileHandler << "/* #define SLOG_BINARY_MODE */\n";

            for (const auto &mapEntry: masterHashMap) {
                auto log = mapEntry.second;
                switch (byteCnt) {
                    case 1:
                        fileHandler << fmt::format("#define {} \"\\x{:02x}{}\"\n",
                                                   log.stagNewName,
                                                   log.stagId,
                                                   log.stagEncArgs);
                        break;
                    case 2:
                        fileHandler << fmt::format("#define {} \"\\x{:02x}\\x{:02x}{}\"\n",
                                                   log.stagNewName,
                                                   (log.stagId >> 8) & 0xFF,
                                                   log.stagId & 0xFF,
                                                   log.stagEncArgs);
                        break;
                    case 3:
                        fileHandler << fmt::format("#define {} \"\\x{:02x}\\x{:02x}\\x{:02x}{}\"\n",
                                                   log.stagNewName,
                                                   (log.stagId >> 16) & 0xFF,
                                                   (log.stagId >> 8) & 0xFF,
                                                   (log.stagId) & 0xFF,
                                                   log.stagEncArgs);
                        break;
                }
            }
        } catch (const std::exception &e) {
            fmt::print(stderr, "Map file generating error\n{}", e.what());
            return InternalStatus::FileGenerationError;
        }

        return InternalStatus::OK;
    }

    ProjectProcessor::OutputStatus ProjectProcessor::ProcessProject(const stdf::path &pth, const uint8_t threadCount) noexcept {
        // find configuration and source files
        if (not is_directory(pth)) {
            fmt::print(stderr, "Project file must be valid path to directory.\n");
            try {
                fmt::print(stderr, "Passed path: {}\n", pth.string());
            }
            catch (...) {
                fmt::print(stderr, "(Unable to print the path)\n");
                return OutputStatus::WrongProjectPath;
            }
        }

        // find a types size configuration file
        stdf::path configPth = pth / "slog/typeConfig.yaml";
        try {
            argToBytesCountConverter = std::make_optional<ArgToBytesCount>(configPth);
        } catch (const ArgToBytesCount::ConfigLoadError& e) {
            fmt::print(stderr, "{}\n", e.what());
            return OutputStatus::InvalidConfiguration;
        } catch (const std::exception& e) {
            fmt::print(stderr, "Unknown error during configuration loading:\n{}\n", e.what());
            return OutputStatus::InvalidConfiguration;
        }

        std::vector<stdf::path> filteredFileNames{};
        auto dirIterator = stdf::recursive_directory_iterator(pth);
        for (const auto &file: dirIterator) {
            // TODO: This IF can be really slow. Check performance and fix if necessary
            [[unlikely]] if (file.path().parent_path().filename() == "slog") {
                dirIterator.disable_recursion_pending();
                continue;
            }
            auto extension = file.path().extension().string();
            if (fileExtensions.contains(extension)) {
                filteredFileNames.emplace_back(file);
            }
        }

        // process project files
        if (filteredFileNames.empty()) {
            fmt::print(stderr, "No files with matching extension in provided directory\n");
            return OutputStatus::EmptyProjectDirectory;
        }

        std::atomic<unsigned> processed = 0, errorCnt = 0;
        ThreadPool<stdf::path> threadPool{threadCount, [this, &errorCnt, &processed](const stdf::path& path){
            auto result = ProcessFile(path);
            if (result != InternalStatus::OK) { errorCnt.fetch_add(1, std::memory_order_relaxed); }
            processed.fetch_add(1, std::memory_order_relaxed);
        }};

        for (auto& file: filteredFileNames) {
            threadPool.AddJob(std::move(file));
        }

        threadPool.JoinAll();

        fmt::print("Processed files: {}/{} (successfully/all)\n", processed.load() - errorCnt.load(), processed.load());

        AssignIDs();
        (void)GenerateMapFile(pth);
        (void)GenerateHeaderFile(pth);

        return ProjectProcessor::OutputStatus::OK;
    }

} // sl