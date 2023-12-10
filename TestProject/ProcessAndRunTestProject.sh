#!/bin/bash
docker run -v $(pwd):/app -w /app shrink-log/build-env cmake-build-release-shrinklog-docker/ProjectProcessor ./TestProject