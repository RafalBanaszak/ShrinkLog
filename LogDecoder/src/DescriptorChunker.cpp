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



