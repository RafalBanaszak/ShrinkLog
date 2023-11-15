//
// Created by rafal on 09.10.2023.
//

#include "ArgToBytesCount.h"
#include "LoadConfig.h"

#include <array>
#include <limits>
#include <stdexcept>

#include <fmt/core.h>

namespace sl {
    ArgToBytesCount::ArgToBytesCount(const std::filesystem::path& pth) {
        //TODO: Implement configuration loading
        auto typesSizeOpt = LoadConfig::LoadTypesSize(pth);
        if(!typesSizeOpt.has_value()){
            throw ConfigLoadError{};
        }
        auto typesSize = typesSizeOpt.value();

        std::array obligatoryFields = {
          "short",
          "int",
          "long",
          "long long",
          "double",
          "long double",
          "wchar_t",
          "intmax_t",
          "size_t",
          "ptrdiff_t",
          "void*",
        };
        std::array allowedValues = {
          1, 2, 4, 8
        };

        for (const auto& field: obligatoryFields) {
            auto it = typesSize.find(field);
            if(it == typesSize.end()) {
                throw ConfigLoadError{};
            }

            if(std::find(allowedValues.begin(), allowedValues.end(), it->second) == allowedValues.end()) {
                throw ConfigLoadError{};
            }
        }

        argToByteMapBasic = {
          {stringToHash("c"), sizeof(char)},
          {stringToHash("s"), '\x7F'},
          {stringToHash("d"), sizeof(int)},
          {stringToHash("i"), sizeof(int)},
          {stringToHash("o"), sizeof(int)},
          {stringToHash("x"), sizeof(int)},
          {stringToHash("X"), sizeof(int)},
          {stringToHash("u"), sizeof(int)},
          {stringToHash("f"), sizeof(double)},
          {stringToHash("F"), sizeof(double)},
          {stringToHash("e"), sizeof(double)},
          {stringToHash("E"), sizeof(double)},
          {stringToHash("a"), sizeof(double)},
          {stringToHash("A"), sizeof(double)},
          {stringToHash("g"), sizeof(double)},
          {stringToHash("G"), sizeof(double)},
          {stringToHash("p"), sizeof(void *)},
        };

        argToByteMapExtended = {
          {stringToHash("hhd"), sizeof(char)},
          {stringToHash("hhi"), sizeof(char)},
          {stringToHash("hho"), sizeof(char)},
          {stringToHash("hhx"), sizeof(char)},
          {stringToHash("hhX"), sizeof(char)},
          {stringToHash("hhu"), sizeof(char)},

          {stringToHash("hd"),  sizeof(short)},
          {stringToHash("hi"),  sizeof(short)},
          {stringToHash("ho"),  sizeof(short)},
          {stringToHash("hx"),  sizeof(short)},
          {stringToHash("hX"),  sizeof(short)},
          {stringToHash("hu"),  sizeof(short)},

          {stringToHash("lc"),  sizeof(wchar_t)},
          {stringToHash("ls"),  '\x7E'},
          {stringToHash("ld"),  sizeof(long)},
          {stringToHash("li"),  sizeof(long)},
          {stringToHash("lo"),  sizeof(long)},
          {stringToHash("lx"),  sizeof(long)},
          {stringToHash("lX"),  sizeof(long)},
          {stringToHash("lu"),  sizeof(long)},
          {stringToHash("lf"),  sizeof(double)},
          {stringToHash("lF"),  sizeof(double)},
          {stringToHash("le"),  sizeof(double)},
          {stringToHash("lE"),  sizeof(double)},
          {stringToHash("la"),  sizeof(double)},
          {stringToHash("lA"),  sizeof(double)},
          {stringToHash("lg"),  sizeof(double)},
          {stringToHash("lG"),  sizeof(double)},

          {stringToHash("lld"), sizeof(long long)},
          {stringToHash("lli"), sizeof(long long)},
          {stringToHash("llo"), sizeof(long long)},
          {stringToHash("llx"), sizeof(long long)},
          {stringToHash("llX"), sizeof(long long)},
          {stringToHash("llu"), sizeof(long long)},

          {stringToHash("jd"),  sizeof(intmax_t)},
          {stringToHash("ji"),  sizeof(intmax_t)},
          {stringToHash("jo"),  sizeof(intmax_t)},
          {stringToHash("jx"),  sizeof(intmax_t)},
          {stringToHash("jX"),  sizeof(intmax_t)},
          {stringToHash("ju"),  sizeof(intmax_t)},

          {stringToHash("zd"),  sizeof(size_t)},
          {stringToHash("zi"),  sizeof(size_t)},
          {stringToHash("zo"),  sizeof(size_t)},
          {stringToHash("zx"),  sizeof(size_t)},
          {stringToHash("zX"),  sizeof(size_t)},
          {stringToHash("zu"),  sizeof(size_t)},

          {stringToHash("td"),  sizeof(ptrdiff_t)},
          {stringToHash("ti"),  sizeof(ptrdiff_t)},
          {stringToHash("to"),  sizeof(ptrdiff_t)},
          {stringToHash("tx"),  sizeof(ptrdiff_t)},
          {stringToHash("tX"),  sizeof(ptrdiff_t)},
          {stringToHash("tu"),  sizeof(ptrdiff_t)},

          {stringToHash("Lf"),  sizeof(long double)},
          {stringToHash("LF"),  sizeof(long double)},
          {stringToHash("Le"),  sizeof(long double)},
          {stringToHash("LE"),  sizeof(long double)},
          {stringToHash("La"),  sizeof(long double)},
          {stringToHash("LA"),  sizeof(long double)},
          {stringToHash("Lg"),  sizeof(long double)},
          {stringToHash("LG"),  sizeof(long double)},
        };
    }

    constexpr uint32_t ArgToBytesCount::stringToHash(const std::string &str) {
        [[unlikely]] if (str.length() == 0) {
            return 0;
        }
        return XXH32(str.data(), str.length(), 0);
    }

    unsigned ArgToBytesCount::GetByteSize(const std::string &base, const std::string &extension) const noexcept {
        unsigned retVal;
        try {
            if (extension.empty()) {
                retVal = argToByteMapBasic.at(stringToHash(base));
            } else {
                retVal = argToByteMapExtended.at(stringToHash(extension + base));
            }
        } catch (std::out_of_range const &e) {
            // From architectural pov regex which matches base and extension should never generate empty base nor
            // match unsupported extension.
            // Being here means we have a bug :(
            fmt::print(stderr, "ERROR: Unknown printf-like argument placeholder: ({}{})\n", extension, base);
            fmt::print(stderr, "Location: {} {} {}\n", __FILE__, __FUNCTION__, __LINE__);
            fmt::print(stderr, "Program will be terminated\n");
            exit(1);
        }
        return retVal;
    }
}

/*
  "Premature optimization is the root of all evil"
                                      Donald Knuth

  I will leave the code below as a monument for my silliness.
*/
//unsigned ArgToBytesCount::GetByteSize(const std::string &base, const std::string &extension) const noexcept {
//    unsigned retVal = std::numeric_limits<unsigned>::max();
//
//    [[likely]] if(!base.empty()) {
//        [[unlikely]] if (base[0]  )
//
//        char baseSelector = base[0] - 'a';
//
//        if (extension.length() == 0){
//            retVal = lt_none[baseSelector];
//        } else if (extension.length() == 1) {
//            char extensionSelector = extension[0] - 'a';
//            [[unlikely]] if (extension[0] == 'L') {
//                retVal = L_size;
//            } else if (lt_psl[extensionSelector] != nullptr) {
//                retVal = (*lt_psl[extensionSelector])[baseSelector];
//            }
//        } else if (extension.length() == 2) {
//            char extensionSelector = extension[0] - 'a';
//
//            [[likely]] if (extensionSelector == extension[1]) {
//                if (extensionSelector == 'h') {
//                    return lt_hh[baseSelector];
//                } else if (extensionSelector == 'l') {
//                    return lt_ll[baseSelector];
//                }
//            }
//        }
//    }
//
//    return retVal;
//}