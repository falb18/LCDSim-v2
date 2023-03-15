add_executable(lcd-example "lcd-example.c")

target_link_libraries(lcd-example PRIVATE ${SDL2_LIBRARIES})
target_link_libraries(lcd-example PRIVATE ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(lcd-example PRIVATE lcdsim)
target_link_libraries(lcd-example PRIVATE avr-hd44780)