//
// Created by rafal on 09.10.2023.
//

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
