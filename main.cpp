#include <chrono>
#include <iostream>

#include "src/ShrinkLog.h"

int main() {
    using std::cout, std::endl;

    cout << "Program start" << endl;

    auto start = std::chrono::steady_clock::now();
    sl::ShrinkLog testObj;
    [[maybe_unused]] auto result = testObj.ProcessProject("../src/testproj/");
    auto end = std::chrono::steady_clock::now();

    cout << endl << endl << "Elapsed time in microseconds: "
         << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
         << " µs" << endl;

    return 0;
}
