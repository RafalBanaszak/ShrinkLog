#include <chrono>

#include <fmt/core.h>

#include "src/ProjectProcessor.h"

int main() {

    fmt::print("Program start\n");

    auto start = std::chrono::steady_clock::now();
    sl::ProjectProcessor testObj;
    [[maybe_unused]] auto result = testObj.ProcessProject("../src/testproj/");
    auto end = std::chrono::steady_clock::now();

    fmt::print("\n\nElapsed time: {}μs\n",
               std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

    return 0;
}
