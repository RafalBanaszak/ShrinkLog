//
// Created by rafal on 25.11.2023.
//

#ifndef SHRINKLOG_FILE_H
#define SHRINKLOG_FILE_H

#include <filesystem>
#include <string>
#include <string_view>

#include <fmt/core.h>


namespace dc {

    class File {
        std::filesystem::path _path;
        std::string _fileContent;

        void copyFromOther(const File&) noexcept;
        void getFromOther(File&&) noexcept;

    public:
        bool valid{};
        enum class Status{
            SUCCESS,
            FAILURE
        };

        explicit File(const std::filesystem::path&) noexcept;
        File(const File&) noexcept;
        File& operator=(const File&) noexcept;
        File(File&&) noexcept;
        File& operator=(File&&) noexcept;

        [[nodiscard]] const std::string& GetContent() const noexcept;
        [[nodiscard]] std::string_view GetContentView() const noexcept;
        Status SaveFile (const std::string& newContent) noexcept;
    };

} // dc

#endif //SHRINKLOG_FILE_H
