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

#include "TextFile.h"

#include <fstream>
#include <sstream>

namespace sl {
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

    std::string TextFile::GetContent() const noexcept {
         return _fileContent;
    }

    void TextFile::SaveFile(const std::string &newContent) {
        _fileContent = newContent;
        std::ofstream ofs{_path, std::ios::trunc};
        ofs << _fileContent;
        ofs.close();
    }

} // sl