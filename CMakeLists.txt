cmake_minimum_required(VERSION 3.0.0)
project(lcdsim-prj VERSION 1.3.1)

message("LCDSimv2 version ${CMAKE_PROJECT_VERSION}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Add SDL2 and SDL2_image libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

# Copy the image to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/res/lcd_layout.bmp
        DESTINATION ${CMAKE_BINARY_DIR}/res/)

# Copy the binary file to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/res/cgrom.bin
        DESTINATION ${CMAKE_BINARY_DIR}/res/)

# Libraries
add_subdirectory(lcdsim)
add_subdirectory(avr-hd44780)

# Main application
add_subdirectory(src)