//
// Created by rafal on 25.11.2023.
//

#ifndef SHRINKLOG_DECODER_H
#define SHRINKLOG_DECODER_H

#include "MessageDescriptor.h"
#include "MessageDescriptorStorage.h"
#include "TextFile.h"

namespace sl {

    class Decoder {
        std::unique_ptr<MessageDescriptorStorage> messageMap;
        unsigned maxMessageIndex;
        unsigned messageIndexBytes;

        template<typename T>
        [[nodiscard]] static T hexStringToType(std::string::const_iterator beginIt, std::string::const_iterator endIt);
        [[nodiscard]] static inline int64_t getSignedArgumentValue(size_t size, uint64_t buffer);
    public:
        explicit Decoder(std::unique_ptr<MessageDescriptorStorage>&& messageMap) noexcept;
        [[nodiscard]] std::string DecodeTextFormat(TextFile&&) noexcept;


        [[nodiscard]] std::string DecodeSingle(std::vector<char> message) noexcept;

    };

} // sl

#endif //SHRINKLOG_DECODER_H
