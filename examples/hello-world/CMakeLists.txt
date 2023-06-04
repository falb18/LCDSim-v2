add_executable(hello-world "hello-world.c")

target_link_libraries(hello-world PRIVATE ${SDL2_LIBRARIES})
target_link_libraries(hello-world PRIVATE ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(hello-world PRIVATE lcdsim)
target_link_libraries(hello-world PRIVATE avr-hd44780)