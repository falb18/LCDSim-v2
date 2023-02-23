cmake_minimum_required(VERSION 3.0.0)
project(lcdsim-prj VERSION 1.1.0)

message("LCDSimv2 version ${PROJECT_VERSION}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Add SDL2 and SDL2_image libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

# Libraries
add_subdirectory(lcdsim)
add_subdirectory(avr-hd44780)

# Main application
add_subdirectory(src)