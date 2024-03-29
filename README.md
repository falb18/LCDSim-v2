# LCDSim-v2

This is an update of the library [LCDSim](https://github.com/dylangageot/LCDSim).
The purpose of this project is to update the library so it uses SDL2, since the previous version of this library uses
SDL v1.2, which is deprecated.

LCDSim is a library, written in **C**, which allows you to simulate a HD44780 LCD 16x2 display on your computer.
The LCD display is capable of showing text thanks to the use of the SDL and SDL-image libraries.

<img src="./docs/imgs/lcdsim-v2-screenshot.png" width="" height="">

The project is built with the following software:
- Meta-build system: cmake 3.28.2 (snap version)
- Build system: GNU Make 4.3
- Compiler: gcc 12.3.0
- OS: Ubuntu 23.04

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

To run the example:
```
cd build/examples/hello-world
./hello-world
```

## Documentation

- docs/cgrom-organization.md
- docs/functions.md

## Resources
- [LCD 16x2 image](http://paulvollmer.net/FritzingParts/parts/lcd-GDM1602K.html)
- [Instruction Set](https://mil.ufl.edu/3744/docs/lcdmanual/commands.html)