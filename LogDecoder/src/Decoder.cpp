//
// Created by rafal on 25.11.2023.
//

#include "Decoder.h"

#include <cstdio>

#include <numeric>
#include <variant>

#include "fmt/core.h"

namespace dc {

    using fmt::print;
    using ArgType = MessageDescriptor::Type;

    Decoder::Decoder(std::unique_ptr<MessageDescriptorStorage> &&messageMap) noexcept:
            messageMap{std::move(messageMap)} {}

    std::string Decoder::DecodeTextFormat(/*TextFile &&file*/) noexcept {

        //ignore the file try to decode on predefined strings
        const auto maxIndex = messageMap->getMaxIndexNumber();
//        std::string alignedInput = "03FFFFFFF000";
        std::string alignedInput = "0A401400000000000040140000000000004014000000000000401400000000000000";

        //Decode index
        auto inputIterator = cbegin(alignedInput);
        uint32_t index = static_cast<uint32_t>(hexStringToU64(inputIterator, inputIterator + 2));
        inputIterator += 2;
        print("Index {} \n", index);

        //decode args
        if (index > maxIndex) {
            return {};
        }
        auto &descriptor = messageMap->GetDescriptor(index);

        std::string decodedLine;
        for (const auto &chunk: descriptor.msgChunks) {
            std::variant<std::string, uint64_t, int64_t, long double, nullptr_t> argumentValue;

            if (chunk.arg.type == ArgType::STRING) {
            //TODO: Implement strings decoder
            } else if (chunk.arg.type == ArgType::SIGNED_INT || chunk.arg.type == ArgType::UNSIGNED_INT) {
                auto charactersToDecode = chunk.arg.size * 2;
                uint64_t buffer = hexStringToU64(inputIterator, inputIterator + charactersToDecode);
                inputIterator += charactersToDecode;

                if (chunk.arg.type == ArgType::SIGNED_INT) {
                    switch (chunk.arg.size) {
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
                    }
                } else {
                    argumentValue = buffer;
                }
            } else if (chunk.arg.type == ArgType::FLOAT) {
                auto charactersToDecode = chunk.arg.size * 2;
                if (chunk.arg.size == 4) {
                    uint32_t buf = hexStringToU64(inputIterator, inputIterator + charactersToDecode);
                    argumentValue = static_cast<long double>(*(reinterpret_cast<float*>(&buf)));

                } else {
                    uint64_t buf = hexStringToU64(inputIterator, inputIterator + charactersToDecode);
                    argumentValue = static_cast<long double>(*(reinterpret_cast<double*>(&buf)));
                }
                inputIterator += charactersToDecode;
            } else if (chunk.arg.type == ArgType::EMPTY) {
                argumentValue = nullptr;
            } else {
                return {};
            }

            std::visit([&decodedLine, &chunk](auto&& arg){
                print("ARG: {}\n", arg);
                constexpr size_t bufferSize = 1000;
                char buffer[bufferSize];
                if constexpr (std::is_same_v<std::string, std::decay<decltype(arg)>>) {
                    std::snprintf(buffer, bufferSize, chunk.message.c_str(), arg.c_str());
                } else if constexpr (std::is_same_v<nullptr_t, std::decay<decltype(arg)>>) {
                    std::snprintf(buffer, bufferSize, chunk.message.c_str());
                } else {
                    std::snprintf(buffer, bufferSize, chunk.message.c_str(), arg);
                }
                decodedLine += buffer;
            }, argumentValue);
        }

        print("Decoded line: {}\n", decodedLine);

        return std::string();
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

} // dc