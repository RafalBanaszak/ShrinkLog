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
