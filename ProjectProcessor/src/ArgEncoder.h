//
// Created by rafal on 09.10.2023.
//

#ifndef SHRINKLOG_ARGENCODER_H
#define SHRINKLOG_ARGENCODER_H

#include <filesystem>
#include <map>
#include <string>

#include <xxhash.h>

namespace sl
{

class ArgEncoder {
public:
    enum class Type{
        UNSIGNED = 'u',
        SIGNED = 'i',
        DOUBLE = 'd',
        STRING = 's'
    };

    struct ArgumentSignature {
        unsigned byteCnt;
        Type type;
    };

    unsigned maxArgSize;

    class ConfigLoadError : public std::exception {
        std::string message = "Unable to load the configuration file with types size.";
    public:
        [[nodiscard]] const char* what () const noexcept override {
            return message.c_str();
        }
    };

    explicit ArgEncoder(const std::filesystem::path& typeConfigPath);
    [[nodiscard]] ArgumentSignature GetByteSize(std::string const& base, std::string const& extension) const noexcept;

private:
    // map stores keys in a form of short hashes (instead strings) to speed up binary tree search
    std::map<uint_fast32_t, ArgumentSignature> argToByteMapBasic;
    std::map<uint_fast32_t, ArgumentSignature> argToByteMapExtended;
    static constexpr uint32_t stringToHash(std::string const& str);
};

}

#endif //SHRINKLOG_ARGENCODER_H
