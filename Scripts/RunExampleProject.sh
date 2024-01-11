#!/bin/bash

# Check if the script is run from its parent directory
script_name=$(basename "$0")
script_path=$(readlink -f "$0")
script_dir=$(dirname "$script_path")
parent_dir=$(dirname "$script_dir")
current_dir=$(pwd)

if [ "$parent_dir" != "$current_dir" ]; then
  echo "This script must be run from its parent directory. Example ./Scripts/${script_name}"
  exit 1
fi

#Run in a docker container following commands
docker run -v $(pwd)/..:/tmp shrink-log/build-env /bin/sh -c \
"                                                                                                                      \
cd /tmp/ShrinkLog/                                                                                                   &&\
# 1. Copy the TargetLib and default configuration to the example project (to avoid files duplication)
mkdir -p ./ExampleProject/slog                                                                                       &&\
cp ./TargetLib/slog/* ./ExampleProject/slog                                                                          &&\
cp ./TargetLib/TypeConfigs/TypeSizeDiscovery.h ./ExampleProject/slog/TypeSizeDiscovery.h                             &&\
cp ./TargetLib/TypeConfigs/default/TypeConfig.yaml ./ExampleProject/slog                                             &&\
# 2. Run the ProjectProcessor to process the example project
./cmake-build-release-shrinklog-docker/ProjectProcessor ./ExampleProject                                             &&\
# 3. Build the example projects
/usr/bin/cmake --build ./cmake-build-release-shrinklog-docker -j --target                                              \
ExampleProject_LogDemo ExampleProject_TypeSizeDiscoveryDemo ExampleProject_Test                                      &&\
# 4. Run the log demo project and save the output to demoRaw.txt
./cmake-build-release-shrinklog-docker/ExampleProject_LogDemo > ./ExampleProject/demoRaw.txt                         &&\
# 5. Decode the log.txt to demoDecoded.txt
./cmake-build-release-shrinklog-docker/LogDecoder ./ExampleProject/slog/map.slog ./ExampleProject/demoRaw.txt         \
    > ./ExampleProject/demoDecoded.txt                                                                               &&\
# 6. Run the type size discovery demo
./cmake-build-release-shrinklog-docker/ExampleProject_TypeSizeDiscoveryDemo > ./ExampleProject/typesSz.txt           &&\
# 7. Run tests
mkdir -p ./ExampleProject/testResults
./cmake-build-release-shrinklog-docker/ExampleProject_Test > ./ExampleProject/testResults/raw.txt                    &&\
./cmake-build-release-shrinklog-docker/LogDecoder ./ExampleProject/slog/map.slog                                       \
    ./ExampleProject/testResults/raw.txt > ./ExampleProject/testResults/decoded.txt
"

source ./Scripts/Utils/PrintStatus.sh
