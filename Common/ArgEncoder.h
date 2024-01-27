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

#ifndef SHRINKLOG_ARGENCODER_H
#define SHRINKLOG_ARGENCODER_H

#include <array>
#include <filesystem>
#include <map>
#include <string>

#include <xxhash.h>

namespace sl
{

class ArgEncoder {
public:
    enum class Type{
        EMPTY = 0,
        UNSIGNED = 'u',
        SIGNED = 'i',
        DOUBLE = 'd',
        STRING = 's'
    };

    struct Argument {
        unsigned byteCnt;
        Type type;
        std::string encoded;
    };

    unsigned maxArgSize;

    explicit ArgEncoder(const std::filesystem::path& typeConfigPath);
    [[nodiscard]] Argument EncodeArg(const std::string &base, const std::string &extension) const;
    [[nodiscard]] static Argument DecodeArg(const std::string& encoded, unsigned maxArgSize = 16);
    [[nodiscard]] Argument GetByteSize(std::string const& base, std::string const& extension) const noexcept;

    class ConfigLoadError : public std::exception {
        std::string message = "Unable to load the configuration file with validTypes size.";
    public:
        [[nodiscard]] const char* what () const noexcept override {
            return message.c_str();
        }
    };

private:
    constexpr static char stringCode = '\x7F';
    // map stores keys in a form of short hashes (instead strings) to speed up binary tree search
    std::map<uint_fast32_t, Argument> argToByteMapBasic;
    std::map<uint_fast32_t, Argument> argToByteMapExtended;
    static constexpr std::array validTypes{'u', 'i', 'd', 's'};
    static constexpr uint32_t stringToHash(std::string const& str);

};

}

#endif //SHRINKLOG_ARGENCODER_H
