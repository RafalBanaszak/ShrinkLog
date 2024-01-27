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

docker run -v $(pwd)/..:/tmp shrink-log/build-env /bin/sh -c \
"                                                                                                             \
cd /tmp/ShrinkLog/                                                                                          &&\
/usr/bin/cmake -DCMAKE_BUILD_TYPE=Release -G Ninja -S . -B ./cmake-build-release-shrinklog-docker           &&\
/usr/bin/cmake --build ./cmake-build-release-shrinklog-docker --target ProjectProcessor -j 18               &&\
/usr/bin/cmake --build ./cmake-build-release-shrinklog-docker --target LogDecoder -j 18                       \
"
