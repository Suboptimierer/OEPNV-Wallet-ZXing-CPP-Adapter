#!/bin/bash
set -e

BUILD_DIR="build"
BIN_NAME="oepnv-wallet-zxing-cpp-adapter"

echo "Starte Linux-Build mit musl-g++..."

rm -rf "$BUILD_DIR"

cmake -S . -B "$BUILD_DIR" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=musl-gcc \
  -DCMAKE_CXX_COMPILER=musl-g++ \
  -DCMAKE_EXE_LINKER_FLAGS="-static" \
  -DBUILD_SHARED_LIBS=OFF \
  -DZxing_INSTALL=OFF \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build "$BUILD_DIR"

echo "Build abgeschlossen. Binary liegt in: $BUILD_DIR/$BIN_NAME"