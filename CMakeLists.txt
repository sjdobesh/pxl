cmake_minimum_required(VERSION 3.7)
project(sdl_pxl)

# compiler info
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_BUILD_TYPE Debug)

# packages
find_package(SDL3   REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
find_package(SDL3_image   REQUIRED)
find_package(SDL3_ttf   REQUIRED)

# directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
include_directories("${PROJECT_SOURCE_DIR}/include")
file(GLOB sources
  "${PROJECT_SOURCE_DIR}/include/*.h"
  "${PROJECT_SOURCE_DIR}/src/*.c")

# compile
add_executable(sdl_pxl ${sources})
target_compile_options(sdl_pxl PRIVATE -g -Wall -Wextra)
target_link_libraries(
  sdl_pxl
  PRIVATE
  SDL3::SDL3 SDL3_image SDL3_ttf
  pngz png m
)
