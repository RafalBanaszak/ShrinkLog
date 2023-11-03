cmake_minimum_required(VERSION 3.25.1)
project(ShrinkLog)

set(CMAKE_CXX_STANDARD 20)



add_executable(ShrinkLog main.cpp
        src/ProjectProcessor.cpp
        src/ProjectProcessor.h
        src/IntfMasterMap.cpp
        src/IntfMasterMap.h
        src/ArgToBytesCount.cpp
        src/ArgToBytesCount.h
        src/ThreadPool.cpp
        src/ThreadPool.h)

target_include_directories(ShrinkLog PRIVATE /usr/include/)
target_link_libraries(ShrinkLog PRIVATE -l:libhs.so.5 -l:libxxhash.so.0 -l:libfmt.so.9)
