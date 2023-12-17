//
// Created by rafal on 25.11.2023.
//

#include "Decoder.h"

#include <cstdio>

#include <numeric>
#include <variant>

#include "fmt/core.h"

namespace sl {

    using fmt::print;

    Decoder::Decoder(std::unique_ptr<MessageDescriptorStorage> &&messageMap) noexcept:
            messageMap{std::move(messageMap)} {
    }

    std::string Decoder::DecodeTextFormat(TextFile &&logFile) noexcept {
        using Type = ArgEncoder::Type;
        const auto maxIndex = messageMap->getMaxIndexNumber();
        std::vector<std::string> outputLines;
        std::string::size_type outputLinesSize = 0;

        std::string logLine;
        std::istringstream issEncodedLog(logFile.GetContent());
        while (std::getline(issEncodedLog, logLine)) {
            //Decode index
            auto inputIterator = cbegin(logLine);
            uint32_t index = static_cast<uint32_t>(hexStringToU64(inputIterator, inputIterator + 2));
            inputIterator += 2;

            //decode args
            if (index > maxIndex) {
                return {};
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
                    uint64_t buffer = hexStringToU64(inputIterator, inputIterator + charactersToDecode);
                    inputIterator += charactersToDecode;

                    if (chunk.arg.type == Type::SIGNED) {
                        argumentValue = getSignedArgumentValue(chunk.arg.byteCnt, buffer);
                    } else {
                        argumentValue = buffer;
                    }
                } else if (chunk.arg.type == Type::DOUBLE) {
                    auto charactersToDecode = chunk.arg.byteCnt * 2;
                    if (chunk.arg.byteCnt == 4) {
                        uint32_t buf = hexStringToU64(inputIterator, inputIterator + charactersToDecode);
                        argumentValue = static_cast<long double>(*(reinterpret_cast<float *>(&buf)));
                    } else {
                        uint64_t buf = hexStringToU64(inputIterator, inputIterator + charactersToDecode);
                        argumentValue = static_cast<long double>(*(reinterpret_cast<double *>(&buf)));
                    }
                    inputIterator += charactersToDecode;
                } else if (chunk.arg.type == Type::EMPTY) {
                    argumentValue = nullptr;
                } else {
                    return {};
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

    uint64_t Decoder::hexStringToU64(const std::string::const_iterator begin, const std::string::const_iterator end) {
        return std::accumulate<std::string::const_iterator, uint64_t>(begin, end, 0u, [](uint64_t value, char c) {
            value <<= 4;
            if (c >= '0' && c <= '9') {
                value |= c - '0';
            } else if (c >= 'A' && c <= 'F') {
                value |= 10 + c - 'A';
            } else if (c >= 'a' && c <= 'f') {
                value |= 10 + c - 'a';
            } else {
                throw std::invalid_argument("Illegal character in a message detected");
            }
            return value;
        });
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