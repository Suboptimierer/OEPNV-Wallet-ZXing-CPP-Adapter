cmake_minimum_required(VERSION 3.15)
project(oepnv-wallet-zxing-cpp-adapter)

# C++17 Standard aktivieren
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Release-Build als Standard
set(CMAKE_BUILD_TYPE Release)

# Statisches Linken und keine Beispiele/Tests
set(BUILD_SHARED_LIBS OFF)
set(BUILD_EXAMPLES OFF)
set(BUILD_TESTING OFF)
set(ZXing_BUILD_WRAPPER OFF)
set(ZXing_INSTALL OFF)

include(FetchContent)

# ZXing-CPP herunterladen (Version 2.3.0)
FetchContent_Declare(
  zxing-cpp
  GIT_REPOSITORY https://github.com/zxing-cpp/zxing-cpp.git
  GIT_TAG        v2.3.0
)
FetchContent_MakeAvailable(zxing-cpp)

# Executables definieren
add_executable(
  oepnv-wallet-zxing-cpp-adapter
  oepnv-wallet-zxing-cpp-adapter.cpp
  base64.cpp
  stb_image.h
  stb_image_write.h
)

# ZXing-Quellverzeichnis als Include-Pfad (für internen Zugriff)
target_include_directories(
  oepnv-wallet-zxing-cpp-adapter PRIVATE
  ${CMAKE_BINARY_DIR}/_deps/zxing-cpp-src/core/src
)

# ZXing statisch linken
target_link_libraries(
  oepnv-wallet-zxing-cpp-adapter
  ZXing::ZXing
)