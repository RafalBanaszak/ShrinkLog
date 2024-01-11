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

./Scripts/BuildDockerImage.sh
./Scripts/BuildShrinkLog.sh
./Scripts/RunExampleProject.sh