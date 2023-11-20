cmake_minimum_required(VERSION 3.25.1)
project(ShrinkLog)

set(CMAKE_CXX_STANDARD 20)

add_executable(ProjectProcessor
        ProjectProcessor/main.cpp
        ProjectProcessor/src/ProjectProcessor.cpp
        ProjectProcessor/src/ArgToBytesCount.cpp
)

target_link_libraries(ProjectProcessor PRIVATE -l:libhs.so.5 -l:libxxhash.so.0 -l:libfmt.so.9 -l:libyaml-cpp.so.0.7.0)
