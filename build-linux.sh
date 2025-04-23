#!/bin/bash
set -e

BIN_NAME="oepnv-wallet-zxing-cpp-adapter"
DOCKER_IMAGE_NAME="zxing-linux-builder"

echo "Starte Linux-Build via Docker..."

rm -rf build

cat <<EOF > Dockerfile.temp
FROM alpine:latest

RUN apk update && apk add --no-cache \\
    g++ \\
    musl-dev \\
    cmake \\
    make \\
    git \\
    build-base

WORKDIR /app

COPY . .

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \\
         -DCMAKE_EXE_LINKER_FLAGS="-static -static-libgcc -static-libstdc++" \\
 && cmake --build build
EOF

docker build --platform=linux/amd64 -f Dockerfile.temp -t "$DOCKER_IMAGE_NAME" .

CONTAINER_ID=$(docker create "$DOCKER_IMAGE_NAME")
docker cp "$CONTAINER_ID":/app/build/$BIN_NAME ./$BIN_NAME
docker rm "$CONTAINER_ID"

rm Dockerfile.temp

chmod +x $BIN_NAME

strip $BIN_NAME

echo "Build abgeschlossen. Binary: $BIN_NAME"