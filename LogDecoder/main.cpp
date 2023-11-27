#include <chrono>

#include <argparse/argparse.hpp>
#include <fmt/core.h>

//TODO: Remove
#include "src/Decoder.h"

int main(int argc, char *argv[]) {
    auto startTime = std::chrono::steady_clock::now();

    argparse::ArgumentParser program("ShrinkLog Log Decoder");
    program.add_argument("LOG_PATH")
      .help("Path to a log file which should be processed.");

    program.add_argument("-j", "--cores")
      .help("Select how many threads use to decode the logs. Default: 1. Range: [1, 255].")
      .default_value(1)
      .scan<'i', int>();

    std::string logPath;
    uint8_t threadCnt;
    try {
        program.parse_args(argc, argv);

        logPath = program.get<std::string>("LOG_PATH");

        const auto tmp = program.get<int>("-j");
        if (tmp > std::numeric_limits<uint8_t>::max() || tmp < 1) {
            throw std::invalid_argument(fmt::format("Thread count must be in [1, 255] range! Provided value: {}\n", tmp));
        }
        threadCnt = tmp;
    }
    catch (const std::exception& err) {
        fmt::print(stderr, "Incorrect input arguments!\n{}\n", err.what());
        logPath = {};
        return 1;
    }

    fmt::print("--------------------\n"
               "Log decoding start\n"
               "Number of threads: {}\n"
               "File path: {}\n"
               "--------------------\n",
               threadCnt, logPath);

    dc::Decoder dec;
    dec.LoadMap("../LogDecoder/tests/slog/map.slog");

    auto stopTime = std::chrono::steady_clock::now();
    fmt::print("Elapsed time: {}ms\n",
               std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());

    return 0;
}