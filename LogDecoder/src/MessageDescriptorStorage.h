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

#ifndef SHRINKLOG_MESSAGEDESCRIPTORSTORAGE_H
#define SHRINKLOG_MESSAGEDESCRIPTORSTORAGE_H

#include "MessageDescriptor.h"
#include "DescriptorChunker.h"
#include "TextFile.h"

#include <filesystem>
#include <ranges>
#include <regex>
#include <vector>

#include "fmt/core.h"

namespace sl {

    class MessageDescriptorStorage {
        const static std::regex linePattern;
        std::vector<MessageDescriptor> _cache;
        ptrdiff_t _linesNumber;

        void LoadMapLine(const std::string &line) noexcept;
    public:
        explicit MessageDescriptorStorage(std::filesystem::path path);
        [[nodiscard]] const MessageDescriptor& GetDescriptor(const unsigned id) const;
        [[nodiscard]] ptrdiff_t getMaxIndexNumber() const;

        friend class DescriptorCleaner;
    };

    using MDS = MessageDescriptorStorage;
} // sl

#endif //SHRINKLOG_MESSAGEDESCRIPTORSTORAGE_H
