#!/bin/bash

# Get the absolute path of the script
script_path=$(readlink -f "$0")
# Get the absolute path of the parent directory
parent_dir=$(dirname "$script_path")
parent_dir=$(dirname "$parent_dir")
# Get the current working directory
current_dir=$(pwd)

echo $script_path
echo $parent_dir
echo $current_dir


# Check if the script is run from its parent directory
if [ "$parent_dir" != "$current_dir" ]; then
  echo "This script must be run from its parent directory. Example ./Scripts/Install.sh"
  exit 1
fi

./Scripts/BuildDockerImage.sh
./Scripts/BuildShrinkLog.sh
./Scripts/RunExampleProject.sh