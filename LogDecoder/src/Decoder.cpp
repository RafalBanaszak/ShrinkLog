//
// Created by rafal on 25.11.2023.
//

#include "Decoder.h"

#include <algorithm>
#include <ranges>

#include "fmt/core.h"

namespace dc {
    using fmt::print;
    const std::regex Decoder::linePattern{R"#((\d+);([^;]*);(.*$))#", std::regex::optimize};
    const std::regex Decoder::argPattern{R"#(\d{1,3}[su])#", std::regex::optimize};
    const std::regex Decoder::substringArgPattern{
      R"#(%[-+ #0]*[\d]*(?:\.\d*)?(?:ll|L)?(?:([cs])|([dioxXup])|(?:[fFeEaAgG])))#", std::regex::optimize};

    void Decoder::LoadMap(const std::filesystem::path &path) {
        File file{path};
        if (not file.valid) {
            throw std::invalid_argument{"Unable to load the map file\n"};
        }

        auto fileContent = file.GetContent();
        linesNumber = std::count(fileContent.cbegin(), fileContent.cend(), '\n') +
                      2; //+1 because msg ids starts from 1 and + 1 in a case there is no extra empty line at the end
        std::istringstream iss(std::move(fileContent));

        {
            std::lock_guard lock{descMtx};
            descriptors.resize(linesNumber);
        }

        for (std::string line; std::getline(iss, line);) {
            LoadMapLine(line);
        }

        for (const auto &line: descriptors) {
            print("DESC: {}\n", line.msgChunks.size());
        }

    }

    void Decoder::LoadMapLine(const std::string &line) noexcept {
        std::smatch match;
        try {
            if (std::regex_match(line, match, linePattern)) {
                unsigned id = std::stoi(match[1]);
                if (id > linesNumber) {
                    throw std::invalid_argument{
                      fmt::format("Map file error. A line index higher than a number of lines {}/{}\n",
                                  id, linesNumber)};
                }

                auto arguments = match[2].str();
                auto message = match[3].str();

                auto messageChunks = DivideMessageToChunks(arguments, message);
                {
                    std::lock_guard lock{descMtx};
                    descriptors[id] = MessageDescriptor(std::move(messageChunks));
                }

            } else {
                //it's abnormal, rare case. Use throw to simplify the function flow.
                throw std::invalid_argument{"A line in map file doesn't match the pattern.\n"};
            }
        } catch (const std::exception &e) {
            print(stderr, "Error during decoding a line in the map file. The line will be ignored.\n");
        }
    }

    std::vector<MessageDescriptor::Chunk>
    Decoder::DivideMessageToChunks(const std::string &arguments, std::string message) {
        std::smatch match;
        std::vector<MessageDescriptor::Chunk> result;
        std::istringstream iss{arguments};

        while (std::regex_search(message, match, substringArgPattern)) {
            const auto &isCharacterLike = match[1].matched;
            const auto &isIntegerLike = match[2].matched;

            std::string messageChunk(message.c_str(), match.position() + match.length());

            if (isCharacterLike) {
                result.emplace_back(std::move(messageChunk), 0, MessageDescriptor::Type::STRING);
            } else { //int like or float like
                std::string arg{};
                std::getline(iss, arg, ' ');
                std::size_t pos;
                auto size = std::stoul(arg, &pos);

                if (isIntegerLike) {
                    auto type = (arg[pos] == 'u' ? MessageDescriptor::Type::UNSIGNED_INT
                                                 : MessageDescriptor::Type::SIGNED_INT);
                    result.emplace_back(std::move(messageChunk), size, type);
                } else /* (isFloatLike) */ {
                    result.emplace_back(std::move(messageChunk), size, MessageDescriptor::Type::FLOAT);
                }
            }

            message = match.suffix();
        }

        if (message.length()) {
            result.emplace_back(std::move(message), 0, MessageDescriptor::Type::EMPTY);
        }

        return result;
    }

} // dc