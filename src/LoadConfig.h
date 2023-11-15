//
// Created by rafal on 15.11.2023.
//

#ifndef SHRINKLOG_LOADCONFIG_H
#define SHRINKLOG_LOADCONFIG_H

#include <map>
#include <optional>
#include <string>
#include <filesystem>

#include <fmt/core.h>
#include <yaml-cpp/yaml.h>

namespace sl {
    struct LoadConfig {
        static std::optional<std::map<std::string, unsigned>> LoadTypesSize(const std::filesystem::path& pth) noexcept {

            std::map<std::string, unsigned> result;
            try {
                YAML::Node config = YAML::LoadFile(pth);

                for(auto it = config.begin(); it != config.end(); ++it) {
                    auto key = it->first.as<std::string>();
                    auto value = it->second.as<unsigned>();
                    result[key] = value;
                }

            } catch (const YAML::BadFile& e) {
                fmt::print(stderr, "Error: Unable to open type size config file '{}'.\n", pth.string());
                return {};
            } catch (const YAML::ParserException& e) {
                fmt::print(stderr, "Error: Yaml parser error at line {}: {}\n{}\n", e.mark.line, e.what(),pth.string());
                return {};
            } catch (const std::exception& e) {
                fmt::print(stderr, "Error: {}\n", e.what());
                return {};
            }

            // Example: Print the map
            for (const auto& pair : result) {
                fmt::print("{}: {}\n", pair.first, pair.second);
            }
            return result;
        }
    };

} // sl

#endif //SHRINKLOG_LOADCONFIG_H
