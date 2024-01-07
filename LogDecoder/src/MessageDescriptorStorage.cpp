//
// Created by rbanasza on 01.12.2023.
//

#include "MessageDescriptorStorage.h"

#include <algorithm>
#include <string>

namespace sl {
    using fmt::print;

    const std::regex MessageDescriptorStorage::linePattern{R"#((\d+);([^;]*);(.*$))#", std::regex::optimize};;

    MessageDescriptorStorage::MessageDescriptorStorage(std::filesystem::path path) {
        // possible exception should be handled at the higher abstraction layer
        TextFile file{path};

        auto fileContent = file.GetContent();
        _linesNumber = std::count(fileContent.cbegin(), fileContent.cend(), '\n') +
                       2; //+1 because msg ids starts from 1 and +1 in a case there is no extra empty line at the end
        std::istringstream iss(std::move(fileContent));

        _cache.resize(_linesNumber);

        for (std::string line; std::getline(iss, line);) {
            LoadMapLine(line);
        }
    }

    void MessageDescriptorStorage::LoadMapLine(const std::string &line) noexcept {
        std::smatch match;
        try {
            if (std::regex_match(line, match, linePattern)) {
                unsigned id = std::stoi(match[1]);
                if (id > _linesNumber) {
                    throw std::invalid_argument{
                            fmt::format("Map file error. A line index higher than a number of lines {}/{}\n",
                                        id, _linesNumber)};
                }
                auto arguments = match[2].str();
                auto message = match[3].str();
                _cache[id] = DescriptorChunker::DivideToChunks(arguments, message);
            } else {
                //it's abnormal, rare case. Use throw to simplify the function flow.
                throw std::invalid_argument{"A line in map file doesn't match the pattern.\n"};
            }
        } catch (const std::exception &e) {
            print(stderr, "Error during decoding a line in the map file. The line will be ignored\nLINE:{}.\n", line);
        }
    }

    const MessageDescriptor &MessageDescriptorStorage::GetDescriptor(const unsigned int id) const {
        return _cache[id];
    }

    ptrdiff_t MessageDescriptorStorage::getMaxIndexNumber() const {
        return _linesNumber;
    }
} // sl