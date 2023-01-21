# LCDSim-v2

This is an update of the library [LCDSim](https://github.com/dylangageot/LCDSim).
The purpose of this project is to update the library so it uses SDL2, since the previous version of this library uses
SDL v1.2, which is deprecated.

LCDSim is a library written in C that allows you to emulate a HD44780 LCD 16x2 display on your computer. The display
function is possible with the use of the SDL library and SDL-image library.

The projects is built using the following:
- Meta-build system: cmake 3.24.0 (snap version)
- Build system: GNU Make 4.1
- Compiler: gcc 7.5.0
- OS: Ubuntu 18.04

The project uses the following libraries:
- SDL2 v2.0.12
- SDL2_image v2.0.3

## Compile the project and run the emulator

To compile the project run the following command:
``` 
cmake -S ./ -B build/
cd build
make
``` 

To run the emulator:
```
cd build/src
./lcdsim
```