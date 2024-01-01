#!/bin/bash

#Run in a docker container following commands
docker run -v $(pwd)/..:/tmp shrink-log/build-env /bin/sh -c \
"                                                                                                             \
cd /tmp/ShrinkLog/                                                                                          &&\
# 1. Copy the TargetLib and default configuration to the test project (to avoid files duplication)
mkdir -p ./ExampleProject/slog                                                                              &&\
cp -r ./TargetLib/slog/* ./ExampleProject/slog                                                              &&\
cp ./TargetLib/TypeConfigs/default/TypeConfig.yaml ./ExampleProject/slog                                    &&\
# 2. Run the ProjectProcessor to process the test project
./cmake-build-release-shrinklog-docker/ProjectProcessor ./ExampleProject                                    &&\
# 3. Build the test project
/usr/bin/cmake --build ./cmake-build-release-shrinklog-docker --target ExampleProject -j 18                 &&\
# 4. Run the test project and save the output to logRaw.txt
./cmake-build-release-shrinklog-docker/ExampleProject > ./ExampleProject/logRaw.txt                         &&\
# 5. Decode the log.txt to logDecoded.txt
./cmake-build-release-shrinklog-docker/LogDecoder ./ExampleProject/slog/map.slog ./ExampleProject/logRaw.txt  \
    > ./ExampleProject/logDecoded.txt                                                                         \
"
