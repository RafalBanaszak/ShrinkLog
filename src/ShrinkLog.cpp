//
// Created by rbanasza on 18.09.2023.
//

#include "ShrinkLog.h"

#include <chrono>
#include <deque>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>


namespace sl {
    std::unordered_set<std::string> ShrinkLog::fileExtensions = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx"};
    std::string ShrinkLog::logMacroName = "LOG";
    std::regex ShrinkLog::pattern{R"###(LOG\(HTAG_[a-zA-Z0-9]{5,6},[ \n]*"[a-zA-Z0-9:%$*.\\ "]*")###"};
    unsigned ShrinkLog::minimumStringWidthToCheck{17};

    ShrinkLog::InternalStatus ShrinkLog::ProcessFile(const stdf::path& pth) const noexcept {
        // open file and load it to string
        using std::fstream;
        fstream fileReader{pth, std::ios::in};
        if (not fileReader.is_open())
        {
            std::cerr << "Unable to open the file: " << absolute(pth) << std::endl;
            return InternalStatus::UnableToOpenFile;
        }

        std::ostringstream fileContent{};
        fileContent << fileReader.rdbuf();

        //find all print macros
        auto printArguments = FindPrintMacros(fileContent.str());
        if (printArguments.empty())
        {
            std::cerr   << "Error during extracting print macros in the file:"  \
                        << absolute(pth)                                     \
                        << std::endl;
            return InternalStatus::UnableToFindPrintArguments;
        }

        return ShrinkLog::InternalStatus::OK;
    }

    std::vector<std::string> ShrinkLog::FindPrintMacros(const std::string& fileContent) const noexcept {
        auto fileRanges = FindUncommentedRanges(fileContent, minimumStringWidthToCheck);

        //TODO: Replace std::regex by https://github.com/intel/hyperscan
        std::smatch match;
        unsigned i = 0;
        for (const auto& fileRange: fileRanges)
        {
            std::string content{fileRange.first, fileRange.second};
            while (regex_search(content, match, pattern))
            {
                std::cout << "Match: " << ++i << " " << match[0] << std::endl;;
                content = match.suffix();
            }
        }

        return std::vector<std::string>();
    }

    std::vector<ShrinkLog::stringRange> ShrinkLog::FindUncommentedRanges(const std::string& fileContent, unsigned minimumRangeWidth) const noexcept {
        std::vector<stringRange> result{};
        result.reserve(100);

        auto it = fileContent.begin() - 1; // -1 to compensate while loop start from ++it
        bool firstSlashDetected{}, inComment{}, blockComment{}, endStarDetected{};
        ssize_t commentStartPosition;

        result.emplace_back(fileContent.begin(), fileContent.begin());

        // Helper function
        // Add second element to range only if the range will be valid and meet minimum width requirements
        auto SetRangeIfValid = [&result, minimumRangeWidth](auto it) {
            auto rangeEnd = it - 2;
            if (std::distance(result.rbegin()->first, rangeEnd) >= minimumRangeWidth)
            {
                result.rbegin()->second = rangeEnd;
            } else {
                result.pop_back();
            }
        };

        while(++it != fileContent.end())
        {
            //READ THIS CODE BOTTOM UP

            //level 3: match block comment last character
            if (endStarDetected && *it == '/')
            {
                //end of block comment
                firstSlashDetected = false;
                inComment = false;
                blockComment = false;
                endStarDetected = false;
                result.emplace_back(it + 1 , it + 1);
            }

            //level 2: match single line comment end or first character of block comment end
            else if (inComment){
                if (blockComment) {
                    if (*it == '*') { endStarDetected = true; }
                }
                else if (*it == '\n') {
                    //end of single line comment
                    firstSlashDetected = false;
                    inComment = false;
                    result.emplace_back(it + 1 , it + 1);
                }
            }

            // level 1: match second comment start character (/ or *)
            else if (firstSlashDetected){
                if      (*it == '*')  { inComment = true; blockComment = true; SetRangeIfValid(it); }
                else if (*it == '/')  { inComment = true;                      SetRangeIfValid(it); }
                else                  { firstSlashDetected = false; }
            }

            // level 0: check if character is a slash
            else if (*it == '/') {
                firstSlashDetected = true;
            }
        }
        //TODO: Remove. Left only for debug
//        int cnt = 0;
//        for (const auto& codeBlock : result)
//        {
//            std::cout << "Comment range: " << std::distance(fileContent.begin(), codeBlock.first) << "-" << std::distance(fileContent.begin(), codeBlock.second) << " " << ++cnt << std::endl;
//            try
//            {
//                std::cout << std::string(codeBlock.first, codeBlock.second) << std::endl;
//            } catch ( ... ) {}
//        }
        return result;
    }

    ShrinkLog::OutputStatus ShrinkLog::ProcessProject(const stdf::path& pth, const unsigned threadCount) noexcept {
        // list all files with matching extensions

        if(not is_directory(pth))
        {
            std::cerr << "Project file must be valid path to directory" << std::endl;
            try
            {
                std::cerr << "Passed path: " << pth << std::endl;
            }
            catch( ... )
            {
                std::cerr << "Unable to print passed path" << std::endl;
                return OutputStatus::WrongProjectPath;
            }
        }

        std::deque<stdf::path> filteredFileNames{};
        for (const auto& file : stdf::directory_iterator(pth))
        {
            auto extension = file.path().extension().string();
            if (fileExtensions.contains(extension))
            {
                filteredFileNames.emplace_back(file);
            }
        }

        // process project files
        // TODO: add multithreading option; remove the (void)threadCount below
        (void)threadCount;

        if (filteredFileNames.empty())
        {
            std::cerr << "No files with matching extension in provided directory" << std::endl;
            return OutputStatus::EmptyProjectDirectory;
        }

        unsigned processed = 0, errorCnt = 0;
        for (const auto& file : filteredFileNames)
        {
            auto result = ProcessFile(file);
            if (result != InternalStatus::OK) { ++errorCnt; }
            ++processed;
        }

        std::cout   << "Processed files: "                  \
                    << (processed - errorCnt) << "/"        \
                    <<  processed << " (successfully/all)";

        return ShrinkLog::OutputStatus::OK;
    }



} // sl