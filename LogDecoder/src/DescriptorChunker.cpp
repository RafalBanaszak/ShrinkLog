//
// Created by rafal on 17.12.2023.
//
#include "DescriptorChunker.h"

#include <regex>

namespace sl::DescriptorChunker {
    MessageDescriptor DivideToChunks(const std::string &arguments, std::string message) {
        const static std::regex substringArgPattern{
          R"#(%[-+ #0]*[\d]*(?:\.\d*)?(?:ll|L)?[csdioxXupfFeEaAgG]())#", std::regex::optimize};;

        MessageDescriptor result;
        std::smatch match;
        std::istringstream iss{arguments};

        while (std::regex_search(message, match, substringArgPattern)) {
            //Message part of the chunk
            const auto &endOfMatchPosition = match.position(1);
            std::string messageChunk(message.c_str(), endOfMatchPosition);

            //Argument part of the chunk
            std::string rawArg{};
            std::getline(iss, rawArg, ' ');
            auto arg = ArgEncoder::DecodeArg(rawArg);
            result.EmplaceChunk(std::move(messageChunk), std::move(arg));

            message = match.suffix();
        }
        if (!message.empty()) {
            result.EmplaceChunk(std::move(message), ArgEncoder::Argument{});
        }
        return result;
    }
}



