//
// Created by rafal on 25.11.2023.
//

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