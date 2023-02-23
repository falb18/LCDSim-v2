cmake_minimum_required(VERSION 3.0.0)

project(avr-hd44780)

add_library(avr-hd44780 STATIC
    "avr-hd44780.c"
)

target_include_directories(
    avr-hd44780 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/
)