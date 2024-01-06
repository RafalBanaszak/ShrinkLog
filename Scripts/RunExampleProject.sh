#!/bin/bash

#Run in a docker container following commands
docker run -v $(pwd)/..:/tmp shrink-log/build-env /bin/sh -c \
"                                                                                                             \
cd /tmp/ShrinkLog/                                                                                          &&\
# 1. Copy the TargetLib and default configuration to the example project (to avoid files duplication)
mkdir -p ./ExampleProject/slog                                                                              &&\
cp ./TargetLib/slog/* ./ExampleProject/slog                                                                 &&\
cp ./TargetLib/TypeConfigs/TypeSizeDiscovery.h ./ExampleProject/slog/TypeSizeDiscovery.h                    &&\
cp ./TargetLib/TypeConfigs/default/TypeConfig.yaml ./ExampleProject/slog                                    &&\
# 2. Run the ProjectProcessor to process the example project
./cmake-build-release-shrinklog-docker/ProjectProcessor ./ExampleProject                                    &&\
# 3. Build the example projects
/usr/bin/cmake --build ./cmake-build-release-shrinklog-docker -j --target                                     \
ExampleProject_LogDemo ExampleProject_TypeSizeDiscoveryDemo ExampleProject_Test                             &&\
# 4. Run the log demo project and save the output to logRaw.txt
./cmake-build-release-shrinklog-docker/ExampleProject_LogDemo > ./ExampleProject/logRaw.txt                 &&\
# 5. Decode the log.txt to logDecoded.txt
./cmake-build-release-shrinklog-docker/LogDecoder ./ExampleProject/slog/map.slog ./ExampleProject/logRaw.txt  \
    > ./ExampleProject/logDecoded.txt                                                                       &&\
# 6. Run the type size discovery demo
./cmake-build-release-shrinklog-docker/ExampleProject_TypeSizeDiscoveryDemo > ./ExampleProject/typesSz.txt  &&\
echo \"SUCCESS!\"
"
