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

#include <chrono>

#include <argparse/argparse.hpp>
#include <fmt/core.h>

#include "src/Decoder.h"
#include "src/MessageDescriptorStorage.h"
#include "src/DescriptorCleaner.h"

int main(int argc, char *argv[]) {
    auto startTime = std::chrono::steady_clock::now();

    argparse::ArgumentParser program("ShrinkLog Log Decoder");
    program.add_argument("MAP_PATH").help("Path to a map.slog file");
    program.add_argument("LOG_PATH").help("Path to a log file which should be processed.");
    program.add_argument("-j", "--cores")
            .help("Select how many threads use to decode the logs. Default: 1. Range: [1, 255].")
            .default_value(1)
            .scan<'i', int>();

    std::string mapPath;
    std::string logPath;
    uint8_t threadCnt;
    try {
        program.parse_args(argc, argv);
        mapPath = program.get<std::string>("MAP_PATH");
        logPath = program.get<std::string>("LOG_PATH");

        const auto tmp = program.get<int>("-j");
        if (tmp > std::numeric_limits<uint8_t>::max() || tmp < 1) {
            throw std::invalid_argument(
                    fmt::format("Thread count must be in [1, 255] range! Provided value: {}\n", tmp));
        }
        threadCnt = tmp;
    }
    catch (const std::exception &err) {
        fmt::print(stderr, "Incorrect input arguments!\n{}\n", err.what());
        return 1;
    }

    fmt::print("--------------------\n"
               "Log decoding start\n"
               "Number of threads: {}\n"
               "Map file path: {}\n"
               "Log file path: {}\n"
               "--------------------\n",
               threadCnt,mapPath, logPath);

    //Load map file
    auto msgDescStorage = std::make_unique<sl::MessageDescriptorStorage>(mapPath);
    //Preprocess map file
    sl::DescriptorCleaner::CleanDescriptors(msgDescStorage);
    //Create decoder and decode the log file
    sl::Decoder dec(std::move(msgDescStorage));
    auto output = dec.DecodeTextFormat(sl::TextFile(logPath));

    auto stopTime = std::chrono::steady_clock::now();
    fmt::print("Decoded logs:\n{}\n", output);
    fmt::print("Elapsed time: {}ms\n",
               std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());



    return 0;
}