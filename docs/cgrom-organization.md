# CGROM organzation

## How the CGROM get the drawing of a character?

To load all the character into the CGROM, LCDSim reads a file called cgrom.bin. This file contains the representation
of a bunch of ASCII code (0x20 ' ' to 0x7D '}' in the original cgrom.bin).

<img src="./imgs/hexview.png" width="" height="450">

I've organised the editor to display 9 bytes on each line. The first byte of the line (in red) is the ASCII code that
we want to draw into the LCD display. The 8 others bytes (in green) are the 8 bytes that correspond to the
pixel-drawing of the character. 8 bytes for 8 lines. The first byte of them is the top of the character and the last
is the bottom.

## How to update the cgrom.bin file?

Take the '!' character. On this [online character generator](https://omerk.github.io/lcdchargen/), I've drawn the
character and I will put it in the cgrom.bin:

<img src="./imgs/hd44780-character-generator.png" width="650" height="">

The generator will give us a code of an array in C. Just look at the binary data wrote in the array and convert it in
hexadecimal to insert this drawing of the character in cgrom.bin.