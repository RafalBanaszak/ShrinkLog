//
// Created by rbanasza on 18.09.2023.
//

#include "ProjectProcessor.h"

#include <algorithm>
#include <chrono>
#include <deque>
#include <fstream>
#include <ranges>
#include <sstream>

#include <fmt/core.h>
#include <hs/hs.h>
#include <xxhash.h>

namespace sl {
    //static variables
    std::unordered_set<std::string> ProjectProcessor::fileExtensions = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp",
                                                                        ".hxx"};
    //LOG\((STAG_[a-zA-Z0-9_]{7})  -- log function and stagView
    //[\s\\\n]*,[\s\\\n]            -- comma with 0-n space/backslash/newline before and/or after
    //(["](?:[^"\\\n]|\\.|\\\n)*["])  -- C string including new lines and escaped quotes
    //[^;]*;                        -- any characters except semicolon until semicolon
    std::regex ProjectProcessor::logPattern{R"###(LOG\((STAG_[a-zA-Z0-9_]{7})[\s\\\n]*,[\s\\\n]*(["](?:[^"\\\n]|\\.|\\\n)*["])[^;]*;)###"};
    std::regex ProjectProcessor::argPattern{R"###(%[-+ #0]*[\d]*(?:\.\d*)?(hh|h|l|ll|j|z|t|L)?([csdioxXufFeEaAgGnp]))###"};

    unsigned ProjectProcessor::minimumStringWidthToCheck{17};

    ProjectProcessor::InternalStatus ProjectProcessor::ProcessFile(const stdf::path &pth) noexcept {
        //TODO: Replace the file loading method to mmap style (faster)
        //open file and load it to string
        using std::fstream;
        fstream fileReader{pth, std::ios::in};
        if (not fileReader.is_open()) {
            fmt::print(stderr, "Unable to open the file: {}\n", absolute(pth).string());
            return InternalStatus::UnableToOpenFile;
        }

        std::ostringstream fileContentOSS{};
        fileContentOSS << fileReader.rdbuf();
        auto fileContentStr = fileContentOSS.str();

        auto printArguments = FindPrints(fileContentStr); //get stagView and message
        if (printArguments.empty()) {
            fmt::print(stderr, "Error during extracting print macros in the file: {} \n", absolute(pth).string());
            return InternalStatus::UnableToFindPrintArguments;
        }

        /* DO NOT OVERTHINK THIS FOR GOD'S SAKE */
        ExtractArguments(printArguments);
        GenerateTagNames(printArguments);
        AppendToMasterHashMap(printArguments);
        ReplaceTags(printArguments);


        return ProjectProcessor::InternalStatus::OK;
    }

    void ProjectProcessor::ReplaceTags(std::vector<LogFunction> logs) const noexcept {
        for (auto& log : logs) {
            std::copy(log.stagNewName.cbegin(), log.stagNewName.cend(), log.stagView.begin());
        }
    }

    void ProjectProcessor::AppendToMasterHashMap(const std::vector<LogFunction> &logs) noexcept {
        std::lock_guard guard(mhmMutex);

        for (auto const& log : logs) {
            masterHashMap[log.message] = log;
        }
    }

    void ProjectProcessor::ExtractArguments(std::vector<LogFunction>& logs) const noexcept {
        //TODO: Replace std::regex by https://github.com/intel/hyperscan
        std::smatch match;
        for (auto& log : logs) {
            std::string content{log.message};
            std::string buffer;
            buffer.reserve(48);

            while (regex_search(content, match, argPattern)) {
                auto byteCnt = argToBytesCountConverter.GetByteSize(match[2], match[1]);
                log.argSzs.push_back(byteCnt);
                buffer.append(fmt::format("\\x{:02x}", byteCnt));

                content = match.suffix();
            }
            log.stagEncArgs = std::move(buffer);
        }
    }

    void ProjectProcessor::GenerateTagNames(std::vector<LogFunction> &logs) const noexcept {
        //helper function to convert to base63 (a-z, A-Z, 0-9, _)
        auto encodeBase63 = [](XXH64_hash_t input) -> std::string {
            constexpr unsigned digitNumber = 7;
            std::string result;
            result.reserve(digitNumber);
            for (int i = 0; i < digitNumber; ++i) {
                char symbol = input % 63;
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

    std::vector<ProjectProcessor::LogFunction> ProjectProcessor::FindPrints(const std::string &fileContent) const noexcept {
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

                result.push_back({ {tagStart, tagEnd}, match.str(2)});
                bufferShift = std::distance(part.begin(), logEnd);
                content = match.suffix();
            }
        }
        return result;
    }

    std::vector<StringSpan>
    ProjectProcessor::FilterUncommentedParts(std::string &fileContent, unsigned minimumRangeWidth) const noexcept {
        std::vector<StringSpan> result{};

        if (fileContent.length() < minimumRangeWidth) {
            return result;
        }

        // helper iterators to mark begin and end of the uncommented string range
        // initially assume the whole file didn't have any parts
        std::string::iterator partBegin, partEnd;
        partBegin = fileContent.begin();
        partEnd = fileContent.end();

        auto it = fileContent.begin() - 1; // -1 to compensate while loop start from ++it
        bool firstSlashDetected{}, inComment{}, blockComment{}, endStarDetected{};

        // Helper function
        // Add second element to range only if the range will be valid and meet minimum width requirements
        const auto SetRangeIfValid = [&result, &partBegin, &partEnd, minimumRangeWidth](auto it) {
            auto rangeEnd = it - 2; // compensate next comment start characters
            if (std::distance(partBegin, rangeEnd) >= minimumRangeWidth) {
                partEnd = rangeEnd;
                result.emplace_back(partBegin, partEnd);
            }
        };

        while (++it != fileContent.end()) {
            //READ THIS CODE BOTTOM UP

            //level 3: match block comment last character
            if (endStarDetected) {
                if (*it == '/'){
                    //end of block comment
                    firstSlashDetected = false;
                    inComment = false;
                    blockComment = false;
                    endStarDetected = false;
                    result.emplace_back(it + 1, it + 1);
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
                    result.emplace_back(it + 1, fileContent.end());
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
                    SetRangeIfValid(it);
                } else { firstSlashDetected = false; }
            }
            // level 0: check if character is a slash
            else if (*it == '/') {
                firstSlashDetected = true;
            }
        }
        return result;
    }

    ProjectProcessor::OutputStatus ProjectProcessor::ProcessProject(const stdf::path &pth, const unsigned threadCount) noexcept {
        // list all files with matching extensions
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

        std::deque<stdf::path> filteredFileNames{};
        for (const auto &file: stdf::directory_iterator(pth)) {
            auto extension = file.path().extension().string();
            if (fileExtensions.contains(extension)) {
                filteredFileNames.emplace_back(file);
            }
        }

        // process project files
        // TODO: add multithreading option; remove the (void)threadCount below
        (void) threadCount;

        if (filteredFileNames.empty()) {
            fmt::print(stderr, "No files with matching extension in provided directory\n");
            return OutputStatus::EmptyProjectDirectory;
        }

        unsigned processed = 0, errorCnt = 0;
        for (const auto &file: filteredFileNames) {
            auto result = ProcessFile(file);
            if (result != InternalStatus::OK) { ++errorCnt; }
            ++processed;
        }

        fmt::print("Processed files: {}/{} (successfully/all)\n", processed - errorCnt, processed);

//        for (const auto& elem : masterHashMap) {
//            fmt::print("0x{:012x} {}\n", elem.first, elem.second);
//        }


        return ProjectProcessor::OutputStatus::OK;
    }



} // sl