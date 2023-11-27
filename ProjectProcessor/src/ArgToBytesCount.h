//
// Created by rafal on 09.10.2023.
//

#ifndef SHRINKLOG_ARGTOBYTESCOUNT_H
#define SHRINKLOG_ARGTOBYTESCOUNT_H

#include <filesystem>
#include <map>
#include <string>

#include <xxhash.h>

namespace sl
{

class ArgToBytesCount {
public:

    enum class Sign{
        UNSIGNED,
        SIGNED
    };

    struct ArgumentSignature {
        unsigned byteCnt;
        Sign sign;
    };

    class ConfigLoadError : public std::exception {
        std::string message = "Unable to load the configuration file with types size.";
    public:
        [[nodiscard]] const char* what () const noexcept override {
            return message.c_str();
        }
    };

    explicit ArgToBytesCount(const std::filesystem::path& pth);
    [[nodiscard]] ArgumentSignature GetByteSize(std::string const& base, std::string const& extension) const noexcept;

private:
    // map stores keys in a form of short hashes (instead strings) to speed up binary tree search
    std::map<uint_fast32_t, ArgumentSignature> argToByteMapBasic;
    std::map<uint_fast32_t, ArgumentSignature> argToByteMapExtended;
    static constexpr uint32_t stringToHash(std::string const& str);
};

}

#endif //SHRINKLOG_ARGTOBYTESCOUNT_H
