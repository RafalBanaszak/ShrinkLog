#!/bin/bash

script_path=$(readlink -f "$0")
script_dir_path=$(dirname "$script_path")
shrinklog_path=$(dirname "$script_dir_path")

docker_command="cd /tmp$(pwd) && /tmp${shrinklog_path}/cmake-build-release-shrinklog-docker/LogDecoder $1 $2"

if [ -n "$3" ]; then
  docker run -v /:/tmp shrink-log/build-env /bin/sh -c "$docker_command" > "$3"
else
  docker run -v /:/tmp shrink-log/build-env /bin/sh -c "$docker_command"
fi

printstatus_path="${script_dir_path}/Utils/PrintStatus.sh"
source "$printstatus_path"