//
// Created by rafal on 25.11.2023.
//

#ifndef SHRINKLOG_DECODER_H
#define SHRINKLOG_DECODER_H

#include "MessageDescriptor.h"
#include "MessageDescriptorStorage.h"
#include "TextFile.h"

namespace dc {

    class Decoder {
        std::unique_ptr<MessageDescriptorStorage> messageMap;

        static inline uint64_t hexStringToU64(std::string::const_iterator begin, std::string::const_iterator end) ;
    public:
        explicit Decoder(std::unique_ptr<MessageDescriptorStorage>&& messageMap) noexcept;
        [[nodiscard]] std::string DecodeTextFormat(/*TextFile&& file*/) noexcept;


        [[nodiscard]] std::string DecodeSingle(std::vector<char> message) noexcept;

    };

} // dc

#endif //SHRINKLOG_DECODER_H
