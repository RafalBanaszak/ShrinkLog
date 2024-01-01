#!/bin/bash

docker run -v $(pwd)/..:/tmp shrink-log/build-env /bin/sh -c \
"                                                                                                             \
cd /tmp/ShrinkLog/                                                                                          &&\
/usr/bin/cmake -DCMAKE_BUILD_TYPE=Release -G Ninja -S . -B ./cmake-build-release-shrinklog-docker           &&\
/usr/bin/cmake --build ./cmake-build-release-shrinklog-docker --target ProjectProcessor -j 18               &&\
/usr/bin/cmake --build ./cmake-build-release-shrinklog-docker --target LogDecoder -j 18                       \
"
