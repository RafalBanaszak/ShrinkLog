//
// Created by rafal on 25.11.2023.
//

#ifndef SHRINKLOG_DECODER_H
#define SHRINKLOG_DECODER_H

#include "MessageDescriptor.h"
#include "MessageDescriptorStorage.h"

namespace dc {

    class Decoder {

    public:
        explicit Decoder(std::unique_ptr<MessageDescriptorStorage>&& messageMap) noexcept;
        [[nodiscard]] std::string DecodeSingle(std::vector<char> message) noexcept;

    };

} // dc

#endif //SHRINKLOG_DECODER_H
