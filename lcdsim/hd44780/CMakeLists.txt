add_library(
    hd44780
    STATIC "hd44780.c"
)

target_include_directories(
    hd44780
    PUBLIC ${PROJECT_SOURCE_DIR}/hd44780/
)

target_link_libraries(
    hd44780
    PRIVATE lcdsim-defs ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES}
)
