add_library(
    lcdsim
    STATIC "lcdsim.c"
)

target_include_directories(
    lcdsim
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(hd44780)
add_subdirectory(lcdsim-defs)

target_link_libraries(
    lcdsim
    PRIVATE lcdsim-defs hd44780
)

target_link_libraries(lcdsim PRIVATE ${SDL2_LIBRARIES})
target_link_libraries(lcdsim PRIVATE ${SDL2_IMAGE_LIBRARIES})