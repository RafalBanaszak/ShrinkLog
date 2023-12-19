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
            throw std::invalid_argument(fmt::format("Thread count must be in [1, 255] range! Provided value: {}\n", tmp));
        }
        threadCnt = tmp;
    }
    catch (const std::exception& err) {
        fmt::print(stderr, "Incorrect input arguments!\n{}\n", err.what());
        projectPath = {};
        return 1;
    }

    // stderr is flushed immediately, stdout is buffered
    // use cout as nasty workaround to avoid printing the message after an exception is thrown from the code below
    auto message = fmt::format("--------------------\n"
                               "Processing start\n"
                               "Number of threads: {}\n"
                               "Project path: {}\n"
                               "--------------------\n",
                               threadCnt, projectPath);
    std::cout << message << std::flush;

    sl::ProjectProcessor testObj;
    auto result = testObj.ProcessProject(projectPath, threadCnt);

    auto stopTime = std::chrono::steady_clock::now();
    fmt::print("Elapsed time: {}ms\n",
               std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());

    return static_cast<int>(result);
}