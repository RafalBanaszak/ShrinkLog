//
// Created by rafal on 25.11.2023.
//

#include "Decoder.h"

#include <numeric>

#include "fmt/core.h"

namespace dc {

    using fmt::print;
    using ArgType = MessageDescriptor::Type;

    Decoder::Decoder(std::unique_ptr<MessageDescriptorStorage> &&messageMap) noexcept:
            messageMap{std::move(messageMap)} {}

    std::string Decoder::DecodeTextFormat(/*TextFile &&file*/) noexcept {

        //ignore the file try to decode on predefined strings
//        3;4s;(the last printf printed %lld characters)\n
        const auto maxIndex = messageMap->getMaxIndexNumber();


//        std::string alignedInput = "03FFFFFFF000";

        std::string alignedInput = "0A401400000000000040140000000000004014000000000000401400000000000000";

        //Decode index
        auto inputIterator = cbegin(alignedInput);
        uint32_t index = hexStringToU64(inputIterator, inputIterator + 2);
        inputIterator += 2;
        print("Index {} \n", index);

        //decode args
        if (index > maxIndex) {
            return {};
        }
        auto &descriptor = messageMap->GetDescriptor(index);

        for (const auto &chunk: descriptor.msgChunks) {
            //TODO: Separate implementation for strings
            if (chunk.arg.type == ArgType::STRING) {

            } else if (chunk.arg.type == ArgType::SIGNED_INT || chunk.arg.type == ArgType::UNSIGNED_INT) {
                auto charactersToDecode = chunk.arg.size * 2;
                uint64_t temp = hexStringToU64(inputIterator, inputIterator + charactersToDecode);
                inputIterator += charactersToDecode;

                if (chunk.arg.type == ArgType::SIGNED_INT) {
                    int64_t out;
                    switch (chunk.arg.size) {
                        case 8:
                            out = static_cast<int64_t>(temp);
                            break;
                        case 4:
                            out = static_cast<int32_t>(temp);
                            break;
                        case 2:
                            out = static_cast<int16_t>(temp);
                            break;
                        case 1:
                            out = static_cast<int8_t>(temp);
                            break;
                    }
                    print("SIGNED_INT {} \n", out);
                } else {
                    print("UNSIGNED_INT {} \n", temp);
                }
            } else if (chunk.arg.type == ArgType::FLOAT) {
                auto charactersToDecode = chunk.arg.size * 2;
                double result;
                if (chunk.arg.size == 4) {
                    uint32_t buf = hexStringToU64(inputIterator, inputIterator + charactersToDecode);
                    float tmp = *(reinterpret_cast<float*>(&buf));
                    result = tmp;
                } else {
                    uint64_t buf = hexStringToU64(inputIterator, inputIterator + charactersToDecode);
                    result = *(reinterpret_cast<double*>(&buf));
                }
                inputIterator += charactersToDecode;
                print("FLOAT {} \n", result);
            } else {
                return {};
            }

        }

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