//
// Created by rafal on 25.11.2023.
//

#ifndef SHRINKLOG_MESSAGEDESCRIPTOR_H
#define SHRINKLOG_MESSAGEDESCRIPTOR_H

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/core.h>

namespace dc {

    struct MessageDescriptor {
        enum class Type {
            UNSIGNED_INT,
            SIGNED_INT,
            FLOAT,
            STRING,
            EMPTY
        };

        struct Chunk {
            std::string message;
            struct {
                unsigned size;
                Type type;
            } arg ;

            Chunk(const std::string& message, const unsigned size, const Type type ) :
            message{message}, arg{.size = size, .type = type} {}

            Chunk(std::string&& message, const unsigned size, const Type type ) :
            message{std::move(message)}, arg{.size = size, .type = type} {}

            // No copy constructor until it isn't required.
            // Chunks should be created, moved to final destination and never touched (copied) again
            Chunk(const Chunk& other) = delete;
            Chunk& operator=(const Chunk& other) = delete;

            Chunk(Chunk&& other) noexcept {
                message = std::move(other.message);
                arg = other.arg;
            }
            Chunk& operator=(Chunk&& other) noexcept {
                message = std::move(other.message);
                arg = other.arg;
                return *this;
            }
        };

        std::vector<Chunk> msgChunks; ///< Log message divided into chunks with only one argument "Val1: %d" "Val2: %f"

        MessageDescriptor() noexcept = default;
        MessageDescriptor(std::vector<Chunk> msgChunks) : msgChunks(std::move(msgChunks)) {}
        MessageDescriptor(const MessageDescriptor&) = delete;
        MessageDescriptor& operator=(const MessageDescriptor&) = delete;
        MessageDescriptor(MessageDescriptor&& other) noexcept {
            msgChunks = std::move(other.msgChunks);
        };
        MessageDescriptor& operator=(MessageDescriptor&& other) noexcept {
            msgChunks = std::move(other.msgChunks);
            return *this;
        }

    };

} // dc

#endif //SHRINKLOG_MESSAGEDESCRIPTOR_H