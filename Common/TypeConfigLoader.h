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

#ifndef SHRINKLOG_TYPECONFIGLOADER_H
#define SHRINKLOG_TYPECONFIGLOADER_H

#include <map>
#include <optional>
#include <string>
#include <filesystem>

#include <fmt/core.h>
#include <yaml-cpp/yaml.h>

namespace sl {
    struct TypeConfigLoader {
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
                fmt::print(stderr, "Error: Unable to open type size config file '{}'.\n", absolute(pth).string());
                return {};
            } catch (const YAML::ParserException& e) {
                fmt::print(stderr, "Error: Yaml parser error at line {}: {}\n{}\n", e.mark.line, e.what(),pth.string());
                return {};
            } catch (const std::exception& e) {
                fmt::print(stderr, "Error: {}\n", e.what());
                return {};
            }

            return result;
        }
    };

} // sl

#endif //SHRINKLOG_TYPECONFIGLOADER_H
