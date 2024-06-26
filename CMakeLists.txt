cmake_minimum_required(VERSION 3.28.0)
project(lcdsim VERSION 2.0.0)

message("LCDSimv2 version ${CMAKE_PROJECT_VERSION}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Add SDL2 and SDL2_image libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

# Copy the image to the build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res/lcd_16x2_green.bmp
        DESTINATION ${CMAKE_BINARY_DIR}/res/)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res/lcd_16x2_blue.bmp
                DESTINATION ${CMAKE_BINARY_DIR}/res/)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res/lcd_20x4_green.png
                DESTINATION ${CMAKE_BINARY_DIR}/res/)

# Copy the binary file to the build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res/cgrom.bin
        DESTINATION ${CMAKE_BINARY_DIR}/res/)

# Libraries
add_subdirectory(lcdsim)

# Example applications
add_subdirectory(examples)