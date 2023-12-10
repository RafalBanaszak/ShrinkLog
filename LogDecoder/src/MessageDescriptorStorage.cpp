//
// Created by rbanasza on 01.12.2023.
//

#include "MessageDescriptorStorage.h"

#include <algorithm>

namespace dc {
    using fmt::print;

    const std::regex MessageDescriptorStorage::linePattern{R"#((\d+);([^;]*);(.*$))#", std::regex::optimize};
    const std::regex MessageDescriptorStorage::substringArgPattern{
            R"#(%[-+ #0]*[\d]*(?:\.\d*)?(?:ll|L)?(?:([cs])|([dioxXup])|(?:[fFeEaAgG])))#", std::regex::optimize};

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

        //TODO: Remove, only for debug
//        for (const auto &line: _cache) {
//            print("DESC: \n");
//            for (const auto &chunk: line.msgChunks) {
//                print("{}\n", chunk.message);
//            }
//            print("\n");
//        }
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
                _cache[id] = DivideMessageToChunks(arguments, message);
            } else {
                //it's abnormal, rare case. Use throw to simplify the function flow.
                throw std::invalid_argument{"A line in map file doesn't match the pattern.\n"};
            }
        } catch (const std::exception &e) {
            print(stderr, "Error during decoding a line in the map file. The line will be ignored.\n");
        }
    }

    MessageDescriptor
    MessageDescriptorStorage::DivideMessageToChunks(const std::string &arguments, std::string message) {
        std::smatch match;
        MessageDescriptor result;
        std::istringstream iss{arguments};

        while (std::regex_search(message, match, substringArgPattern)) {
            const auto &isCharacterLike = match[1].matched;
            const auto &isIntegerLike = match[2].matched;

            std::string messageChunk(message.c_str(), match.position() + match.length());

            if (isCharacterLike) {
                result.EmplaceChunk(std::move(messageChunk), 0, MessageDescriptor::Type::STRING);
            } else { //int like or float like
                std::string arg{};
                std::getline(iss, arg, ' ');
                std::size_t pos;
                auto size = std::stoul(arg, &pos);

                if (isIntegerLike) {
                    auto type = (arg[pos] == 'u' ? MessageDescriptor::Type::UNSIGNED_INT
                                                 : MessageDescriptor::Type::SIGNED_INT);
                    result.EmplaceChunk(std::move(messageChunk), size, type);
                } else /* (isFloatLike) */ {
                    result.EmplaceChunk(std::move(messageChunk), size, MessageDescriptor::Type::FLOAT);
                }
            }

            message = match.suffix();
        }

        if (message.length()) {
            result.EmplaceChunk(std::move(message), 0, MessageDescriptor::Type::EMPTY);
        }

        return result;
    }

    const MessageDescriptor &MessageDescriptorStorage::GetDescriptor(const unsigned int id) const {
        return _cache[id];
    }

    ptrdiff_t MessageDescriptorStorage::getMaxIndexNumber() const {
        return _linesNumber;
    }
} // dc