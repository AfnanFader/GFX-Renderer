#!/bin/bash

PROJECT_DIR="$(pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
CONFIG="$1"

# Check if build directory exist and start build
if [ -d "$BUILD_DIR" ]; then
    cmake --build ${BUILD_DIR}
    echo "[INFO] Build Success! .."
else
    echo "[ERROR] Build Configuration was not found!"
fi

cd "$PROJECT_DIR" || exit
