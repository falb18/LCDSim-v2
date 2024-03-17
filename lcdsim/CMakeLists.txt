cmake_minimum_required(VERSION 3.28.0)

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

target_compile_definitions(lcdsim
    PRIVATE LCDSIM_VERSION="${PROJECT_VERSION}"
    PRIVATE LCDSIM_CGROM_BIN="${CMAKE_BINARY_DIR}/res/cgrom.bin"
    PRIVATE LCDSIM_LCD_GREEN="${CMAKE_BINARY_DIR}/res/lcd_16x2_green.bmp"
    PRIVATE LCDSIM_LCD_BLUE="${CMAKE_BINARY_DIR}/res/lcd_16x2_blue.bmp"
)