#include <chrono>

#include <argparse/argparse.hpp>
#include <fmt/core.h>

#include "src/ProjectProcessor.h"

int main(int argc, char *argv[]) {
//    argparse::ArgumentParser program("program_name");
//    program.add_argument("square")
//      .help("display the square of a given integer")
//      .scan<'i', int>();
//
//    try {
//        program.parse_args(argc, argv);
//    }
//    catch (const std::exception& err) {
//        std::cerr << err.what() << std::endl;
//        std::cerr << program;
//        return 1;
//    }
//
//    auto input = program.get<int>("square");
//    std::cout << (input * input) << std::endl;

    fmt::print("Program start\n");

    auto start = std::chrono::steady_clock::now();
    sl::ProjectProcessor testObj;
    [[maybe_unused]] auto result = testObj.ProcessProject("../ProjectProcessor/tests/testproj/", 12);
    auto end = std::chrono::steady_clock::now();

    fmt::print("\n\nElapsed time: {}ms\n",
               std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    return 0;
}