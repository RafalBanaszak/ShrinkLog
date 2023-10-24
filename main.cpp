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


//TODO: Remove notes below
/*
 * New algorithm idea
 *
 * main stages
 * - every thread extracts logs from a file. Calculates partial define values (argument list).
 * - every thread at the end updates the master hash map (mhm). It checks if given log is present:
 *      - yes -> get the log define name (7 digit base 63)
 *      - no ->  generate define name and define value (concat number and the previously calculated argument list)
 *          put them into mhm
 * - every thread updates the LOG defines
 * - when all threads finished their job:
 *      - get map size and calculate required number of bytes to represent a log
 *      - generate log numbers
 *      - generate the define map
 *
 *
 * - to modify source file we require: define_names
 * - to generate master define file we require: define_names, partial_define_values, log_ids
 * - to generate decoder file we require: log_ids, log_message, partial_define_values
 * -
 *
 */
