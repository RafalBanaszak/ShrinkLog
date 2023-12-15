cmake_minimum_required(VERSION 3.25.1)
project(ShrinkLog)

set(CMAKE_CXX_STANDARD 20)

add_executable(ProjectProcessor
        ProjectProcessor/main.cpp
        ProjectProcessor/src/ProjectProcessor.cpp
        Common/ArgEncoder.cpp
)
target_include_directories(ProjectProcessor PRIVATE
        ProjectProcessor/src
        Common
)
target_link_libraries(ProjectProcessor PRIVATE -l:libhs.so.5 -l:libxxhash.so.0 -l:libfmt.so.9 -l:libyaml-cpp.so.0.7.0)
#target_link_libraries(ProjectProcessor PRIVATE -l:libhs.a -l:libxxhash.a -l:libfmt.so -l:libyaml-cpp.a)

add_executable(LogDecoder LogDecoder/main.cpp
        LogDecoder/src/Decoder.cpp
        LogDecoder/src/Decoder.h
        LogDecoder/src/TextFile.cpp
        LogDecoder/src/TextFile.h
        LogDecoder/src/MessageDescriptor.cpp
        LogDecoder/src/MessageDescriptor.h
        LogDecoder/src/MessageDescriptorStorage.cpp
        LogDecoder/src/MessageDescriptorStorage.h
        Common/ArgEncoder.cpp
        LogDecoder/src/DescriptorCleaner.cpp
        LogDecoder/src/DescriptorCleaner.h
)

target_include_directories(LogDecoder PRIVATE
        ProjectProcessor/src
        Common
)

target_link_libraries(LogDecoder PRIVATE -l:libhs.so.5 -l:libxxhash.so.0 -l:libfmt.so.9 -l:libyaml-cpp.so.0.7.0)

add_executable(TestProject TestProject/main.c)
