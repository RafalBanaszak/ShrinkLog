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

#ifndef SHRINKLOG_MESSAGEDESCRIPTOR_H
#define SHRINKLOG_MESSAGEDESCRIPTOR_H

#include "ArgEncoder.h"

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/core.h>

namespace sl {

    struct MessageDescriptor {
        struct Chunk {
            using Argument = ArgEncoder::Argument;
            std::string message;
            Argument arg;

            Chunk(const std::string& message, const Argument& arg) :
            message{message}, arg{arg} {}

            Chunk(std::string&& message, Argument&& arg) noexcept :
            message{std::move(message)}, arg{std::move(arg)} {}

            // No copy constructor until it isn't required.
            // Chunks should be created, moved to final destination and never touched (copied) again
            Chunk(const Chunk& other) = delete;
            Chunk& operator=(const Chunk& other) = delete;
            Chunk(Chunk&& other) noexcept = default;
            Chunk& operator=(Chunk&& other) noexcept = default;
        };

        std::vector<Chunk> msgChunks; ///< Log message divided into chunks with only one argument "Val1: %d" "Val2: %f"

        MessageDescriptor() noexcept = default;
        explicit MessageDescriptor(std::vector<Chunk>&& msgChunks) : msgChunks(std::move(msgChunks)) {}

        MessageDescriptor(const MessageDescriptor&) = delete;
        MessageDescriptor& operator=(const MessageDescriptor&) = delete;
        MessageDescriptor(MessageDescriptor&& other) noexcept = default;
        MessageDescriptor& operator=(MessageDescriptor&& other) noexcept = default;

        template<typename... Args>
        void EmplaceChunk(Args&&... args) noexcept {
            msgChunks.emplace_back(std::forward<Args>(args)...);
        }

    };

} // sl

#endif //SHRINKLOG_MESSAGEDESCRIPTOR_