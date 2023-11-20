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
          {stringToHash("c"), 1},
          {stringToHash("s"), '\x7F'},
          {stringToHash("d"), typesSize["int"]},
          {stringToHash("i"), typesSize["int"]},
          {stringToHash("o"), typesSize["int"]},
          {stringToHash("x"), typesSize["int"]},
          {stringToHash("X"), typesSize["int"]},
          {stringToHash("u"), typesSize["int"]},
          {stringToHash("f"), typesSize["double"]},
          {stringToHash("F"), typesSize["double"]},
          {stringToHash("e"), typesSize["double"]},
          {stringToHash("E"), typesSize["double"]},
          {stringToHash("a"), typesSize["double"]},
          {stringToHash("A"), typesSize["double"]},
          {stringToHash("g"), typesSize["double"]},
          {stringToHash("G"), typesSize["double"]},
          {stringToHash("p"), typesSize["void*"]},
        };

        argToByteMapExtended = {
          {stringToHash("hhd"), 1},
          {stringToHash("hhi"), 1},
          {stringToHash("hho"), 1},
          {stringToHash("hhx"), 1},
          {stringToHash("hhX"), 1},
          {stringToHash("hhu"), 1},

          {stringToHash("hd"),  typesSize["short"]},
          {stringToHash("hi"),  typesSize["short"]},
          {stringToHash("ho"),  typesSize["short"]},
          {stringToHash("hx"),  typesSize["short"]},
          {stringToHash("hX"),  typesSize["short"]},
          {stringToHash("hu"),  typesSize["short"]},

          {stringToHash("lc"),  typesSize["wchar_t"]},
          {stringToHash("ls"),  '\x7E'},
          {stringToHash("ld"),  typesSize["long"]},
          {stringToHash("li"),  typesSize["long"]},
          {stringToHash("lo"),  typesSize["long"]},
          {stringToHash("lx"),  typesSize["long"]},
          {stringToHash("lX"),  typesSize["long"]},
          {stringToHash("lu"),  typesSize["long"]},
          {stringToHash("lf"),  typesSize["double"]},
          {stringToHash("lF"),  typesSize["double"]},
          {stringToHash("le"),  typesSize["double"]},
          {stringToHash("lE"),  typesSize["double"]},
          {stringToHash("la"),  typesSize["double"]},
          {stringToHash("lA"),  typesSize["double"]},
          {stringToHash("lg"),  typesSize["double"]},
          {stringToHash("lG"),  typesSize["double"]},

          {stringToHash("lld"), typesSize["long long"]},
          {stringToHash("lli"), typesSize["long long"]},
          {stringToHash("llo"), typesSize["long long"]},
          {stringToHash("llx"), typesSize["long long"]},
          {stringToHash("llX"), typesSize["long long"]},
          {stringToHash("llu"), typesSize["long long"]},

          {stringToHash("jd"),  typesSize["intmax_t"]},
          {stringToHash("ji"),  typesSize["intmax_t"]},
          {stringToHash("jo"),  typesSize["intmax_t"]},
          {stringToHash("jx"),  typesSize["intmax_t"]},
          {stringToHash("jX"),  typesSize["intmax_t"]},
          {stringToHash("ju"),  typesSize["intmax_t"]},

          {stringToHash("zd"),  typesSize["size_t"]},
          {stringToHash("zi"),  typesSize["size_t"]},
          {stringToHash("zo"),  typesSize["size_t"]},
          {stringToHash("zx"),  typesSize["size_t"]},
          {stringToHash("zX"),  typesSize["size_t"]},
          {stringToHash("zu"),  typesSize["size_t"]},

          {stringToHash("td"),  typesSize["ptrdiff_t"]},
          {stringToHash("ti"),  typesSize["ptrdiff_t"]},
          {stringToHash("to"),  typesSize["ptrdiff_t"]},
          {stringToHash("tx"),  typesSize["ptrdiff_t"]},
          {stringToHash("tX"),  typesSize["ptrdiff_t"]},
          {stringToHash("tu"),  typesSize["ptrdiff_t"]},

          {stringToHash("Lf"),  typesSize["long double"]},
          {stringToHash("LF"),  typesSize["long double"]},
          {stringToHash("Le"),  typesSize["long double"]},
          {stringToHash("LE"),  typesSize["long double"]},
          {stringToHash("La"),  typesSize["long double"]},
          {stringToHash("LA"),  typesSize["long double"]},
          {stringToHash("Lg"),  typesSize["long double"]},
          {stringToHash("LG"),  typesSize["long double"]},
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