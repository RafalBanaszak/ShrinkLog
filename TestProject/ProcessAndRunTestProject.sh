#!/bin/bash
docker run -v $(pwd)/..:/tmp shrink-log/build-env /bin/sh -c \
"/tmp/ShrinkLog/cmake-build-release-shrinklog-docker/ProjectProcessor /tmp/ShrinkLog/TestProject; \
/usr/bin/cmake --build /tmp/ShrinkLog/cmake-build-release-shrinklog-docker --target TestProject -j 18; \
/tmp/ShrinkLog/cmake-build-release-shrinklog-docker/TestProject > /tmp/ShrinkLog/TestProject/log.txt"
