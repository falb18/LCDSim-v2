add_executable(lcd-example "lcd-example.c")

# Copy the image to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/res/lcd_layout.bmp
        DESTINATION ${CMAKE_BINARY_DIR}/res/)

# Copy the binary file to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/res/cgrom.bin
        DESTINATION ${CMAKE_BINARY_DIR}/res/)

target_link_libraries(lcd-example PRIVATE ${SDL2_LIBRARIES})
target_link_libraries(lcd-example PRIVATE ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(lcd-example PRIVATE lcdsim)