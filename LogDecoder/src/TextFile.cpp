//
// Created by rafal on 25.11.2023.
//

#include "TextFile.h"

#include <fstream>
#include <sstream>

namespace dc {
    using fmt::print;

     TextFile::TextFile(const std::filesystem::path &path) {
        _path = path;
        std::ifstream fs{_path};

        if (not fs.good()){
            throw std::invalid_argument("A file with given name does not exist.");
        }
        std::ostringstream oss{};
        oss << fs.rdbuf();
        fs.close();

        _fileContent = oss.str();
    }

    const std::string& TextFile::GetContent() const noexcept {
         return _fileContent;
    }

    void TextFile::SaveFile(const std::string &newContent) {
        _fileContent = newContent;
        std::ofstream ofs{_path, std::ios::trunc};
        ofs << _fileContent;
        ofs.close();
    }

} // dc