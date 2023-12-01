cmake_minimum_required(VERSION 3.25.1)
project(ShrinkLog)

set(CMAKE_CXX_STANDARD 20)

add_executable(ProjectProcessor
        ProjectProcessor/main.cpp
        ProjectProcessor/src/ProjectProcessor.cpp
        ProjectProcessor/src/ArgToBytesCount.cpp
)
target_link_libraries(ProjectProcessor PRIVATE -l:libhs.so.5 -l:libxxhash.so.0 -l:libfmt.so.9 -l:libyaml-cpp.so.0.7.0)

add_executable(LogDecoder LogDecoder/main.cpp
        LogDecoder/src/Decoder.cpp
        LogDecoder/src/Decoder.h
        LogDecoder/src/File.cpp
        LogDecoder/src/File.h
        LogDecoder/src/MessageDescriptor.cpp
        LogDecoder/src/MessageDescriptor.h
        LogDecoder/src/MessageDescriptorStorage.cpp
        LogDecoder/src/MessageDescriptorStorage.h
)

target_link_libraries(LogDecoder PRIVATE -l:libhs.so.5 -l:libxxhash.so.0 -l:libfmt.so.9 -l:libyaml-cpp.so.0.7.0)