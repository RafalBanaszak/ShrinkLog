//
// Created by rafal on 09.10.2023.
//

#include "ArgEncoder.h"
#include "LoadConfig.h"

#include <stdexcept>
#include <ranges>

#include <fmt/core.h>

namespace sl {
    ArgEncoder::ArgEncoder(const std::filesystem::path& typeConfigPath) {
        auto typesSizeOpt = LoadConfig::LoadTypesSize(typeConfigPath);
        if(!typesSizeOpt.has_value()){
            throw ConfigLoadError{};
        }
        auto typesSize = typesSizeOpt.value();

        constexpr std::array obligatoryFields = {
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
        constexpr std::array allowedValues = {
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

        //TODO: Implement error handling when maxArgSize it too large.
        maxArgSize = std::max_element(cbegin(typesSize), cend(typesSize),
                                      [](const auto &element1, const auto &element2) {
                                          return element1.second < element2.second;
                                      })->second;

        argToByteMapBasic = {
          {stringToHash("c"), {1,                   Type::UNSIGNED}},
          {stringToHash("s"), {'\x7F',              Type::STRING}},
          {stringToHash("d"), {typesSize["int"],    Type::SIGNED}},
          {stringToHash("i"), {typesSize["int"],    Type::SIGNED}},
          {stringToHash("o"), {typesSize["int"],    Type::UNSIGNED}},
          {stringToHash("x"), {typesSize["int"],    Type::UNSIGNED}},
          {stringToHash("X"), {typesSize["int"],    Type::UNSIGNED}},
          {stringToHash("u"), {typesSize["int"],    Type::UNSIGNED}},
          {stringToHash("f"), {typesSize["double"], Type::DOUBLE}},
          {stringToHash("F"), {typesSize["double"], Type::DOUBLE}},
          {stringToHash("e"), {typesSize["double"], Type::DOUBLE}},
          {stringToHash("E"), {typesSize["double"], Type::DOUBLE}},
          {stringToHash("a"), {typesSize["double"], Type::DOUBLE}},
          {stringToHash("A"), {typesSize["double"], Type::DOUBLE}},
          {stringToHash("g"), {typesSize["double"], Type::DOUBLE}},
          {stringToHash("G"), {typesSize["double"], Type::DOUBLE}},
          {stringToHash("p"), {typesSize["void*"],  Type::UNSIGNED}},
        };

        argToByteMapExtended = {
          {stringToHash("hhd"), {1,                        Type::SIGNED}},
          {stringToHash("hhi"), {1,                        Type::SIGNED}},
          {stringToHash("hho"), {1,                        Type::UNSIGNED}},
          {stringToHash("hhx"), {1,                        Type::UNSIGNED}},
          {stringToHash("hhX"), {1,                        Type::UNSIGNED}},
          {stringToHash("hhu"), {1,                        Type::UNSIGNED}},

          {stringToHash("hd"),  {typesSize["short"],       Type::SIGNED}},
          {stringToHash("hi"),  {typesSize["short"],       Type::SIGNED}},
          {stringToHash("ho"),  {typesSize["short"],       Type::UNSIGNED}},
          {stringToHash("hx"),  {typesSize["short"],       Type::UNSIGNED}},
          {stringToHash("hX"),  {typesSize["short"],       Type::UNSIGNED}},
          {stringToHash("hu"),  {typesSize["short"],       Type::UNSIGNED}},

          {stringToHash("lc"),  {typesSize["wchar_t"],     Type::UNSIGNED}},
          {stringToHash("ls"),  {'\x7E',                   Type::UNSIGNED}},
          {stringToHash("ld"),  {typesSize["long"],        Type::SIGNED}},
          {stringToHash("li"),  {typesSize["long"],        Type::SIGNED}},
          {stringToHash("lo"),  {typesSize["long"],        Type::UNSIGNED}},
          {stringToHash("lx"),  {typesSize["long"],        Type::UNSIGNED}},
          {stringToHash("lX"),  {typesSize["long"],        Type::UNSIGNED}},
          {stringToHash("lu"),  {typesSize["long"],        Type::UNSIGNED}},
          {stringToHash("lf"),  {typesSize["double"],      Type::DOUBLE}},
          {stringToHash("lF"),  {typesSize["double"],      Type::DOUBLE}},
          {stringToHash("le"),  {typesSize["double"],      Type::DOUBLE}},
          {stringToHash("lE"),  {typesSize["double"],      Type::DOUBLE}},
          {stringToHash("la"),  {typesSize["double"],      Type::DOUBLE}},
          {stringToHash("lA"),  {typesSize["double"],      Type::DOUBLE}},
          {stringToHash("lg"),  {typesSize["double"],      Type::DOUBLE}},
          {stringToHash("lG"),  {typesSize["double"],      Type::DOUBLE}},

          {stringToHash("lld"), {typesSize["long long"],   Type::SIGNED}},
          {stringToHash("lli"), {typesSize["long long"],   Type::SIGNED}},
          {stringToHash("llo"), {typesSize["long long"],   Type::UNSIGNED}},
          {stringToHash("llx"), {typesSize["long long"],   Type::UNSIGNED}},
          {stringToHash("llX"), {typesSize["long long"],   Type::UNSIGNED}},
          {stringToHash("llu"), {typesSize["long long"],   Type::UNSIGNED}},

          {stringToHash("jd"),  {typesSize["intmax_t"],    Type::SIGNED}},
          {stringToHash("ji"),  {typesSize["intmax_t"],    Type::SIGNED}},
          {stringToHash("jo"),  {typesSize["intmax_t"],    Type::UNSIGNED}},
          {stringToHash("jx"),  {typesSize["intmax_t"],    Type::UNSIGNED}},
          {stringToHash("jX"),  {typesSize["intmax_t"],    Type::UNSIGNED}},
          {stringToHash("ju"),  {typesSize["intmax_t"],    Type::UNSIGNED}},

          {stringToHash("zd"),  {typesSize["size_t"],      Type::SIGNED}},
          {stringToHash("zi"),  {typesSize["size_t"],      Type::SIGNED}},
          {stringToHash("zo"),  {typesSize["size_t"],      Type::UNSIGNED}},
          {stringToHash("zx"),  {typesSize["size_t"],      Type::UNSIGNED}},
          {stringToHash("zX"),  {typesSize["size_t"],      Type::UNSIGNED}},
          {stringToHash("zu"),  {typesSize["size_t"],      Type::UNSIGNED}},

          {stringToHash("td"),  {typesSize["ptrdiff_t"],   Type::SIGNED}},
          {stringToHash("ti"),  {typesSize["ptrdiff_t"],   Type::SIGNED}},
          {stringToHash("to"),  {typesSize["ptrdiff_t"],   Type::UNSIGNED}},
          {stringToHash("tx"),  {typesSize["ptrdiff_t"],   Type::UNSIGNED}},
          {stringToHash("tX"),  {typesSize["ptrdiff_t"],   Type::UNSIGNED}},
          {stringToHash("tu"),  {typesSize["ptrdiff_t"],   Type::UNSIGNED}},

          {stringToHash("Lf"),  {typesSize["long double"], Type::DOUBLE}},
          {stringToHash("LF"),  {typesSize["long double"], Type::DOUBLE}},
          {stringToHash("Le"),  {typesSize["long double"], Type::DOUBLE}},
          {stringToHash("LE"),  {typesSize["long double"], Type::DOUBLE}},
          {stringToHash("La"),  {typesSize["long double"], Type::DOUBLE}},
          {stringToHash("LA"),  {typesSize["long double"], Type::DOUBLE}},
          {stringToHash("Lg"),  {typesSize["long double"], Type::DOUBLE}},
          {stringToHash("LG"),  {typesSize["long double"], Type::DOUBLE}},
        };
    }

    constexpr uint32_t ArgEncoder::stringToHash(const std::string &str) {
        [[unlikely]] if (str.length() == 0) {
            return 0;
        }
        return XXH32(str.data(), str.length(), 0);
    }

    ArgEncoder::ArgumentSignature ArgEncoder::GetByteSize(const std::string &base, const std::string &extension) const noexcept {
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
//unsigned ArgEncoder::GetByteSize(const std::string &base, const std::string &extension) const noexcept {
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