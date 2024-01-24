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
target_link_libraries(ProjectProcessor PRIVATE
        -l:libxxhash.so.0
        -l:libfmt.so.9
        -l:libyaml-cpp.so.0.7.0)

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
        LogDecoder/src/DescriptorChunker.h
        LogDecoder/src/DescriptorChunker.cpp
)
target_include_directories(LogDecoder PRIVATE
        ProjectProcessor/src
        Common
)
target_link_libraries(LogDecoder PRIVATE
        -l:libxxhash.so.0
        -l:libfmt.so.9
        -l:libyaml-cpp.so.0.7.0)


set(EXAMPLE_PROJECT_SRC ExampleProject/main.c)
set(EXAMPLE_PROJECT_INC_DIR ExampleProject/subdir)

add_executable(ExampleProject_LogDemo ${EXAMPLE_PROJECT_SRC})
target_include_directories(ExampleProject_LogDemo PRIVATE ${EXAMPLE_PROJECT_INC_DIR})
target_compile_definitions(ExampleProject_LogDemo PRIVATE -DSLOG_LOG_EXAMPLE -DSLOG_LONG_DOUBLE_ENABLE)

add_executable(ExampleProject_TypeSizeDiscoveryDemo ${EXAMPLE_PROJECT_SRC})
target_include_directories(ExampleProject_TypeSizeDiscoveryDemo PRIVATE ${EXAMPLE_PROJECT_INC_DIR})
target_compile_definitions(ExampleProject_TypeSizeDiscoveryDemo PRIVATE -DSLOG_DISCOVER_EXAMPLE -DSLOG_LONG_DOUBLE_ENABLE)

add_executable(ExampleProject_Test ${EXAMPLE_PROJECT_SRC})
target_include_directories(ExampleProject_Test PRIVATE ${EXAMPLE_PROJECT_INC_DIR})
target_compile_definitions(ExampleProject_Test PRIVATE -DSLOG_TEST -DSLOG_LONG_DOUBLE_ENABLE)

# Dummy project to allow IDEs like CLion properly view the files
add_library(TargetLibDummy INTERFACE
        TargetLib/slog/ShrinkLog.h
        TargetLib/TypeConfigs/TypeSizeDiscovery.h)