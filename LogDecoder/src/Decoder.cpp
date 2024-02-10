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

#include "Decoder.h"

#include <cstdio>

#include <numeric>
#include <variant>

#include "fmt/core.h"

namespace sl {

    using fmt::print;

    Decoder::Decoder(std::unique_ptr<MessageDescriptorStorage> &&messageMap) noexcept: messageMap{
        std::move(messageMap)
    } {
        maxMessageIndex = this->messageMap->getMaxIndexNumber();
        if (maxMessageIndex <= 0xFFU) {
            messageIndexBytes = 1;
        } else if (maxMessageIndex <= 0xFFFFU) {
            messageIndexBytes = 2;
        } else if (maxMessageIndex <= 0xFFFFFFU) {
            messageIndexBytes = 3;
        } else {
            print(stderr, "The maximum message index is greater than 2^24 - 1 ({}). Indexes must be stored in 3 bytes or fewer.\n", maxMessageIndex);
            std::terminate();
        }
    }

    std::string Decoder::DecodeTextFormat(TextFile &&logFile) noexcept {
        using Type = ArgEncoder::Type;
        std::vector<std::string> outputLines;
        std::string::size_type outputLinesSize = 0;

        std::string logLine;
        std::istringstream issEncodedLog(logFile.GetContent());
        while (std::getline(issEncodedLog, logLine)) {
            //Decode index
            auto inputIterator = cbegin(logLine);
            auto index = hexStringToType<uint32_t>(inputIterator, inputIterator + 2 * messageIndexBytes);
            inputIterator += 2;

            //decode args
            if (index > maxMessageIndex) {
                print(stderr, "The decoded message index is greater than maximum message index({}/{}). The decoding process will be terminated\n", index, maxMessageIndex);
                std::terminate();
            }
            auto &descriptor = messageMap->GetDescriptor(index);

            std::string decodedLine;
            for (const auto &chunk: descriptor.msgChunks) {
                std::variant<std::string, uint64_t, int64_t, long double, nullptr_t> argumentValue;
                if (chunk.arg.type == Type::STRING) {
                    auto stringEnd = std::find(inputIterator, logLine.cend(), '\0');
                    if (stringEnd != logLine.cend()) {
                        argumentValue = std::string(inputIterator, stringEnd);
                        inputIterator = stringEnd + 1;
                    } else {
                        break;
                    }
                } else if (chunk.arg.type == Type::SIGNED || chunk.arg.type == Type::UNSIGNED) {
                    auto charactersToDecode = chunk.arg.byteCnt * 2;
                    auto buffer = hexStringToType<uint64_t>(inputIterator, inputIterator + charactersToDecode);
                    inputIterator += charactersToDecode;

                    if (chunk.arg.type == Type::SIGNED) {
                        argumentValue = getSignedArgumentValue(chunk.arg.byteCnt, buffer);
                    } else {
                        argumentValue = buffer;
                    }
                } else if (chunk.arg.type == Type::DOUBLE) {
                    auto charactersToDecode = chunk.arg.byteCnt * 2;
                    if (chunk.arg.byteCnt == 4) {
                        //TODO: Does it make any sense?
                        argumentValue = hexStringToType<float>(inputIterator, inputIterator + charactersToDecode);
                    } else if (chunk.arg.byteCnt == 8) {
                        argumentValue = hexStringToType<double>(inputIterator, inputIterator + charactersToDecode);
                    } else {
                        argumentValue = hexStringToType<long double>(inputIterator, inputIterator + charactersToDecode);
                    }
                    inputIterator += charactersToDecode;
                } else if (chunk.arg.type == Type::EMPTY) {
                    argumentValue = nullptr;
                } else {
                    print(stderr, "Urecognized argument typei({}). The decoding process will be terminated\n", static_cast<char>(chunk.arg.type));
                    std::terminate();
                }

                std::visit([&decodedLine, &chunk](auto &&arg) {
                    constexpr size_t bufferSize = 1000;
                    char buffer[bufferSize];
                    if constexpr (std::is_same_v<std::string, std::decay_t<decltype(arg)>>) {
                        std::snprintf(buffer, bufferSize, chunk.message.c_str(), arg.c_str());
                    } else if constexpr (std::is_same_v<nullptr_t, std::decay_t<decltype(arg)>>) {
                        std::snprintf(buffer, bufferSize, chunk.message.c_str());
                    } else {
                        std::snprintf(buffer, bufferSize, chunk.message.c_str(), arg);
                    }
                    decodedLine += buffer;
                }, argumentValue);
            }
            outputLinesSize += decodedLine.size();
            outputLines.emplace_back(std::move(decodedLine));
        }

        std::string outputString;
        outputString.reserve(outputLinesSize);
        for (const auto& line: outputLines) {
            outputString += line;
        }
        return outputString;
    }

    template<typename T>
    T Decoder::hexStringToType(const std::string::const_iterator beginIt, const std::string::const_iterator endIt) {
        [[unlikely]] if (std::distance(beginIt, endIt) > sizeof(T) * 2) {
            throw std::invalid_argument("Hex string too long");
        }

        auto HexToByte = [](const char hex) {
            if (hex >= '0' && hex <= '9') {
                return hex - '0';
            }
            if (hex >= 'A' && hex <= 'F') {
                return 10 + hex - 'A';
            }
            if (hex >= 'a' && hex <= 'f') {
                return 10 + hex - 'a';
            }
            throw std::invalid_argument("Illegal character in a message detected");
        };

        T result{};
        const auto rptr = reinterpret_cast<unsigned char*>(&result);
        unsigned byteCnt{};
        for(auto it = beginIt; it != endIt; it += 2) {
            uint_fast8_t byte{};
            byte |= HexToByte(*it) << 4;
            byte |= HexToByte(*(it+1));
            rptr[byteCnt++] = byte;
        }
        return result;
    }

    int64_t Decoder::getSignedArgumentValue(size_t size, uint64_t buffer) {
        int64_t argumentValue = 0;
        switch (size) {
            case 8:
                argumentValue = static_cast<int64_t>(buffer);
                break;
            case 4:
                argumentValue = static_cast<int64_t>(static_cast<int32_t>(buffer));
                break;
            case 2:
                argumentValue = static_cast<int64_t>(static_cast<int16_t>(buffer));
                break;
            case 1:
                argumentValue = static_cast<int64_t>(static_cast<int8_t>(buffer));
                break;
            default:
                throw std::invalid_argument("Size is not valid.");
        }
        return argumentValue;
    }
} // sl
