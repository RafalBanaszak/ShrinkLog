//
// Created by rbanasza on 01.12.2023.
//

#ifndef SHRINKLOG_MESSAGEDESCRIPTORSTORAGE_H
#define SHRINKLOG_MESSAGEDESCRIPTORSTORAGE_H

#include "MessageDescriptor.h"
#include "TextFile.h"

#include <filesystem>
#include <ranges>
#include <regex>
#include <vector>

#include "fmt/core.h"

namespace sl {

    class MessageDescriptorStorage {
        const static std::regex linePattern;
        const static std::regex substringArgPattern;

        std::vector<MessageDescriptor> _cache;
        ptrdiff_t _linesNumber;

        void LoadMapLine(const std::string &line) noexcept;
        static MessageDescriptor DivideMessageToChunks(const std::string &arguments, std::string message);

    public:
        explicit MessageDescriptorStorage(std::filesystem::path path);
        [[nodiscard]] const MessageDescriptor& GetDescriptor(const unsigned id) const;
        [[nodiscard]] ptrdiff_t getMaxIndexNumber() const;
    };

    using MDS = MessageDescriptorStorage;
} // sl

#endif //SHRINKLOG_MESSAGEDESCRIPTORSTORAGE_H
