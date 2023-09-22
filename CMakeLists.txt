cmake_minimum_required(VERSION 3.16)
project(ShrinkLog)

set(CMAKE_CXX_STANDARD 20)

add_executable(ShrinkLog main.cpp
        src/ShrinkLog.cpp
        src/ShrinkLog.h
        src/IntfMasterMap.cpp
        src/IntfMasterMap.h)


target_link_libraries(ShrinkLog -l:libhs.so.5)