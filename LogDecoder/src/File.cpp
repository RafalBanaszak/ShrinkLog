//
// Created by rafal on 25.11.2023.
//

#include "File.h"

#include <fstream>
#include <sstream>

namespace dc {
    using fmt::print;

     File::File(const std::filesystem::path &path) noexcept {
        try {
            _path = path;
            std::ifstream fs{_path};

            if (not fs.good()){
                throw std::invalid_argument("A file with given name does not exist.");
            }
            std::ostringstream oss{};
            oss << fs.rdbuf();
            fs.close();

            _fileContent = oss.str();
            valid = true;
        } catch (const std::exception& e) {
            try {
                print(stderr, "Unable to open the file: {}\n{}\n", _path.string(), e.what());
            } catch (const std::exception &e2) {
                print(stderr, "Unable to open the file: FILE_NAME_ERROR\n{}\n{}\n", e.what(), e2.what());
            }
            _path = std::filesystem::path{};
            _fileContent = std::string{};
            valid = false;
        }
    }

    void File::copyFromOther(const File& other) noexcept {
        try {
            _path = other._path;
            _fileContent = other._fileContent;
            valid = other.valid;
        } catch (const std::exception& e) {
            try {
                print(stderr, "Unable to copy the File: {}\n{}\n", _path.string(), e.what());
            } catch (const std::exception &e2) {
                print(stderr, "Unable to copy the File: FILE_NAME_ERROR\n{}\n{}\n", e.what(), e2.what());
            }
            _path = std::filesystem::path{};
            _fileContent = std::string{};
            valid = false;
        }
    }

    void File::getFromOther(File&& other) noexcept {
        _path = std::move(other._path);
        _fileContent = std::move(other._fileContent);
        valid = other.valid;
    }

    File::File(const File &other) noexcept {
        copyFromOther(other);
    }

    File& File::operator=(const File& other) noexcept {
        copyFromOther(other);
        return *this;
    }

    File::File(File &&other) noexcept {
        getFromOther(std::move(other));
    }

    File& File::operator=(File&& other) noexcept {
        getFromOther(std::move(other));
        return *this;
    }

    const std::string& File::GetContent() const noexcept {
        return _fileContent;
    }

    std::string_view File::GetContentView() const noexcept {
        return std::string_view{_fileContent};
    }

    File::Status File::SaveFile(const std::string &newContent) noexcept {
        Status status;
        try {
            if (not valid) {
                return File::Status::FAILURE;
            }
            _fileContent = newContent;
            std::ofstream ofs{_path, std::ios::trunc};
            ofs << _fileContent;
            ofs.close();

            status = File::Status::SUCCESS;
        } catch (const std::exception& e) {
            print(stderr, "Unable to save a new content to the file: {}\n{}\n", _path.string(), e.what());
            status = File::Status::FAILURE;
        }

        return status;
    }




} // dc