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

#include "MessageDescriptorStorage.h"

#include <algorithm>
#include <string>

namespace sl {
    using fmt::print;

    const std::regex MessageDescriptorStorage::linePattern{R"#((\d+);([^;]*);(.*$))#", std::regex::optimize};

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