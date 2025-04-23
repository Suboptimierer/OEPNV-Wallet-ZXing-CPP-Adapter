#!/bin/bash
set -e

BIN_NAME="oepnv-wallet-zxing-cpp-adapter"

echo "Starte macOS-Build mit gcc-14..."

rm -rf build

cmake -S . -B build \
  -DCMAKE_C_COMPILER=gcc-14 \
  -DCMAKE_CXX_COMPILER=g++-14 \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build build --config Release -j8

cp "build/$BIN_NAME" .

chmod +x $BIN_NAME

strip $BIN_NAME

rm -rf build

echo "Build abgeschlossen. Binary: $BIN_NAME"