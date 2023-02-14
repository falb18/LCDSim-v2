cmake_minimum_required(VERSION 3.0.0)

project(lcdsim VERSION ${PROJECT_VERSION})

add_library(lcdsim STATIC
    "lcdsim.c"
    "hd44780/hd44780.c"
)

target_include_directories(
    lcdsim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/
    lcdsim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/hd44780/
    lcdsim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lcdsim-defs/
)

target_link_libraries(lcdsim PRIVATE ${SDL2_LIBRARIES})
target_link_libraries(lcdsim PRIVATE ${SDL2_IMAGE_LIBRARIES})