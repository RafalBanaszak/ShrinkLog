//
// Created by rafal on 09.10.2023.
//

#ifndef SHRINKLOG_ARGTOBYTESCOUNT_H
#define SHRINKLOG_ARGTOBYTESCOUNT_H

#include <cstdint>

#include <map>
#include <string>

#include <xxhash.h>

namespace sl
{

class ArgToBytesCount {
    // map stores keys in a form of short hashes (instead strings) to speed up binary tree search
    std::map<uint_fast32_t, unsigned> argToByteMapBasic;
    std::map<uint_fast32_t, unsigned> argToByteMapExtended;
    static constexpr uint32_t stringToHash(std::string const& str);

public:
    ArgToBytesCount() noexcept;
    [[nodiscard]] unsigned GetByteSize(std::string const& base, std::string const& extension) const noexcept;

};

}

#endif //SHRINKLOG_ARGTOBYTESCOUNT_H
