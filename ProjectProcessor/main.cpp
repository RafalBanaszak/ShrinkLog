#include <chrono>

#include <argparse/argparse.hpp>
#include <fmt/core.h>

#include "src/ProjectProcessor.h"

int main(int argc, char *argv[]) {
    auto startTime = std::chrono::steady_clock::now();

    argparse::ArgumentParser program("ShrinkLog Project Processor");
    program.add_argument("PROJECT_PATH")
      .help("Path to a project which should be processed.");

    program.add_argument("-j", "--cores")
        .help("Select how many threads use to process the project. Default: 8. Range: [1, 255].")
        .default_value(8)
        .scan<'i', int>();

    std::string projectPath;
    uint8_t threadCnt;
    try {
        program.parse_args(argc, argv);
        projectPath = program.get<std::string>("PROJECT_PATH");
        const auto tmp = program.get<int>("-j");
        if (tmp > std::numeric_limits<uint8_t>::max() || tmp < 1) {
            throw std::invalid_argument("Thread count must be in [1, 255] range!");
        }
        threadCnt = tmp;
    }
    catch (const std::exception& err) {
        fmt::print(stderr, "Incorrect input arguments!\n{}\n", err.what());
        projectPath = {};
        return 1;
    }

    fmt::print("--------------------\n"
               "Processing start\n"
               "Number of threads: {}\n"
               "Project path: {}\n"
               "--------------------\n",
               threadCnt, projectPath);
    sl::ProjectProcessor testObj;
    [[maybe_unused]] auto result = testObj.ProcessProject(projectPath, threadCnt);

    auto stopTime = std::chrono::steady_clock::now();
    fmt::print("Elapsed time: {}ms\n",
               std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());

    return 0;
}