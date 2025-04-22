#!/bin/bash
set -e

BUILD_DIR="build"
BIN_NAME="oepnv-wallet-zxing-cpp-adapter"

echo "Starte macOS-Build mit gcc-14..."

rm -rf "$BUILD_DIR"

cmake -S . -B "$BUILD_DIR" \
  -DCMAKE_C_COMPILER=gcc-14 \
  -DCMAKE_CXX_COMPILER=g++-14 \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build "$BUILD_DIR" --config Release -j8

echo "Build abgeschlossen. Binary liegt in: $BUILD_DIR/$BIN_NAME"