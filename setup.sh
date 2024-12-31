#!/bin/bash

PROJECT_DIR="$(pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
CONFIG="$1"

# Check if build directory exist and do clean up if needed.
if [ -d "$BUILD_DIR" ] && [ "$1" == "clean_all" ]; then
    echo "[INFO] Cleaning up : $BUILD_DIR"
    rm -rf "${BUILD_DIR}"
fi

# Configure the project
echo "[INFO] Configure CMake Project"
cmake -S . -B ${BUILD_DIR} -DCMAKE_BUILD_TYPE=Debug

# Return to the project directory
echo "[INFO] Configure Success! .."
cd "$PROJECT_DIR" || exit
