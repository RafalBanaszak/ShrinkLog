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

        [[nodiscard]] static inline uint64_t hexStringToU64(std::string::const_iterator begin, std::string::const_iterator end);
        [[nodiscard]] static inline int64_t getSignedArgumentValue(size_t size, uint64_t buffer);
        void replaceSpecialChars(void) noexcept;
    public:
        explicit Decoder(std::unique_ptr<MessageDescriptorStorage>&& messageMap) noexcept;
        [[nodiscard]] std::string DecodeTextFormat(TextFile&&) noexcept;


        [[nodiscard]] std::string DecodeSingle(std::vector<char> message) noexcept;

    };

} // sl

#endif //SHRINKLOG_DECODER_H
