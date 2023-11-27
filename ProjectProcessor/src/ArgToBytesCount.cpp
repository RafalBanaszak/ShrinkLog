//
// Created by rafal on 09.10.2023.
//

#include "ArgToBytesCount.h"
#include "LoadConfig.h"

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
          {stringToHash("c"), {1,                      Sign::UNSIGNED}},
          {stringToHash("s"), {'\x7F',                 Sign::UNSIGNED}},
          {stringToHash("d"), {typesSize["int"],       Sign::SIGNED}},
          {stringToHash("i"), {typesSize["int"],       Sign::SIGNED}},
          {stringToHash("o"), {typesSize["int"],       Sign::UNSIGNED}},
          {stringToHash("x"), {typesSize["int"],       Sign::UNSIGNED}},
          {stringToHash("X"), {typesSize["int"],       Sign::UNSIGNED}},
          {stringToHash("u"), {typesSize["int"],       Sign::UNSIGNED}},
          {stringToHash("f"), {typesSize["double"],    Sign::SIGNED}},
          {stringToHash("F"), {typesSize["double"],    Sign::SIGNED}},
          {stringToHash("e"), {typesSize["double"],    Sign::SIGNED}},
          {stringToHash("E"), {typesSize["double"],    Sign::SIGNED}},
          {stringToHash("a"), {typesSize["double"],    Sign::SIGNED}},
          {stringToHash("A"), {typesSize["double"],    Sign::SIGNED}},
          {stringToHash("g"), {typesSize["double"],    Sign::SIGNED}},
          {stringToHash("G"), {typesSize["double"],    Sign::SIGNED}},
          {stringToHash("p"), {typesSize["void*"],     Sign::UNSIGNED}},
        };

        argToByteMapExtended = {
          {stringToHash("hhd"), {1, Sign::SIGNED}},
          {stringToHash("hhi"), {1, Sign::SIGNED}},
          {stringToHash("hho"), {1, Sign::UNSIGNED}},
          {stringToHash("hhx"), {1, Sign::UNSIGNED}},
          {stringToHash("hhX"), {1, Sign::UNSIGNED}},
          {stringToHash("hhu"), {1, Sign::UNSIGNED}},

          {stringToHash("hd"),  {typesSize["short"], Sign::SIGNED}},
          {stringToHash("hi"),  {typesSize["short"], Sign::SIGNED}},
          {stringToHash("ho"),  {typesSize["short"], Sign::UNSIGNED}},
          {stringToHash("hx"),  {typesSize["short"], Sign::UNSIGNED}},
          {stringToHash("hX"),  {typesSize["short"], Sign::UNSIGNED}},
          {stringToHash("hu"),  {typesSize["short"], Sign::UNSIGNED}},

          {stringToHash("lc"),  {typesSize["wchar_t"],  Sign::UNSIGNED}},
          {stringToHash("ls"),  {'\x7E',                Sign::UNSIGNED}},
          {stringToHash("ld"),  {typesSize["long"],     Sign::SIGNED}},
          {stringToHash("li"),  {typesSize["long"],     Sign::SIGNED}},
          {stringToHash("lo"),  {typesSize["long"],     Sign::UNSIGNED}},
          {stringToHash("lx"),  {typesSize["long"],     Sign::UNSIGNED}},
          {stringToHash("lX"),  {typesSize["long"],     Sign::UNSIGNED}},
          {stringToHash("lu"),  {typesSize["long"],     Sign::UNSIGNED}},
          {stringToHash("lf"),  {typesSize["double"],   Sign::SIGNED}},
          {stringToHash("lF"),  {typesSize["double"],   Sign::SIGNED}},
          {stringToHash("le"),  {typesSize["double"],   Sign::SIGNED}},
          {stringToHash("lE"),  {typesSize["double"],   Sign::SIGNED}},
          {stringToHash("la"),  {typesSize["double"],   Sign::SIGNED}},
          {stringToHash("lA"),  {typesSize["double"],   Sign::SIGNED}},
          {stringToHash("lg"),  {typesSize["double"],   Sign::SIGNED}},
          {stringToHash("lG"),  {typesSize["double"],   Sign::SIGNED}},

          {stringToHash("lld"), {typesSize["long long"], Sign::SIGNED}},
          {stringToHash("lli"), {typesSize["long long"], Sign::SIGNED}},
          {stringToHash("llo"), {typesSize["long long"], Sign::UNSIGNED}},
          {stringToHash("llx"), {typesSize["long long"], Sign::UNSIGNED}},
          {stringToHash("llX"), {typesSize["long long"], Sign::UNSIGNED}},
          {stringToHash("llu"), {typesSize["long long"], Sign::UNSIGNED}},

          {stringToHash("jd"),  {typesSize["intmax_t"], Sign::SIGNED}},
          {stringToHash("ji"),  {typesSize["intmax_t"], Sign::SIGNED}},
          {stringToHash("jo"),  {typesSize["intmax_t"], Sign::UNSIGNED}},
          {stringToHash("jx"),  {typesSize["intmax_t"], Sign::UNSIGNED}},
          {stringToHash("jX"),  {typesSize["intmax_t"], Sign::UNSIGNED}},
          {stringToHash("ju"),  {typesSize["intmax_t"], Sign::UNSIGNED}},

          {stringToHash("zd"),  {typesSize["size_t"], Sign::SIGNED}},
          {stringToHash("zi"),  {typesSize["size_t"], Sign::SIGNED}},
          {stringToHash("zo"),  {typesSize["size_t"], Sign::UNSIGNED}},
          {stringToHash("zx"),  {typesSize["size_t"], Sign::UNSIGNED}},
          {stringToHash("zX"),  {typesSize["size_t"], Sign::UNSIGNED}},
          {stringToHash("zu"),  {typesSize["size_t"], Sign::UNSIGNED}},

          {stringToHash("td"),  {typesSize["ptrdiff_t"], Sign::SIGNED}},
          {stringToHash("ti"),  {typesSize["ptrdiff_t"], Sign::SIGNED}},
          {stringToHash("to"),  {typesSize["ptrdiff_t"], Sign::UNSIGNED}},
          {stringToHash("tx"),  {typesSize["ptrdiff_t"], Sign::UNSIGNED}},
          {stringToHash("tX"),  {typesSize["ptrdiff_t"], Sign::UNSIGNED}},
          {stringToHash("tu"),  {typesSize["ptrdiff_t"], Sign::UNSIGNED}},

          {stringToHash("Lf"),  {typesSize["long double"], Sign::SIGNED}},
          {stringToHash("LF"),  {typesSize["long double"], Sign::SIGNED}},
          {stringToHash("Le"),  {typesSize["long double"], Sign::SIGNED}},
          {stringToHash("LE"),  {typesSize["long double"], Sign::SIGNED}},
          {stringToHash("La"),  {typesSize["long double"], Sign::SIGNED}},
          {stringToHash("LA"),  {typesSize["long double"], Sign::SIGNED}},
          {stringToHash("Lg"),  {typesSize["long double"], Sign::SIGNED}},
          {stringToHash("LG"),  {typesSize["long double"], Sign::SIGNED}},
        };
    }

    constexpr uint32_t ArgToBytesCount::stringToHash(const std::string &str) {
        [[unlikely]] if (str.length() == 0) {
            return 0;
        }
        return XXH32(str.data(), str.length(), 0);
    }

    ArgToBytesCount::ArgumentSignature ArgToBytesCount::GetByteSize(const std::string &base, const std::string &extension) const noexcept {
        ArgumentSignature retVal;
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