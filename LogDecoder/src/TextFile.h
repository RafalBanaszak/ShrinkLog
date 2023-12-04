//
// Created by rafal on 25.11.2023.
//

#ifndef SHRINKLOG_TEXTFILE_H
#define SHRINKLOG_TEXTFILE_H

#include <filesystem>
#include <string>
#include <string_view>

#include <fmt/core.h>


namespace dc {

    class TextFile {
        std::filesystem::path _path;
        std::string _fileContent;

    public:
        explicit TextFile(const std::filesystem::path&);
        TextFile(const TextFile&) = delete;
        TextFile& operator=(const TextFile&) = delete;
        TextFile(TextFile&&) noexcept = default;
        TextFile& operator=(TextFile&&) noexcept = default;

        [[nodiscard]] const std::string& GetContent() const noexcept;
        void SaveFile (const std::string& newContent);
    };

} // dc

#endif //SHRINKLOG_TEXTFILE_H
