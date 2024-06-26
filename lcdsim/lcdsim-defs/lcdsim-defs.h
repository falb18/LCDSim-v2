#ifndef LCDSIM_DEFS_H
#define LCDSIM_DEFS_H

#include <SDL2/SDL.h>

/**
 * The width in pixels of the LCD font.
 * Usually the font size for alphanumerc LCDs is 5x8.
 */
#define LCD_FONT_WIDTH  5

/**
 * The height in pixels of the LCD font.
 * Usually the font size for alphanumerc LCDs is 5x8.
 */
#define LCD_FONT_HEIGHT 8

/**
 * The total number of character codes defined in the CGROM.
 * The number comes from table 4, p.18 of the datasheet.
 * Although it support 240 characters, the file only has 128.
 */
#define NUM_CHARACTER_CODES 128

/**
 * The number of bytes for each character.
 * The format is the following:
 *  - 1 byte for the character code
 *  - 8 bytes for the character pattern. 
 */
#define BYTES_PER_CHARACTER 9

/**
 * The number of bytes for each character pattern.
 * The number comes from table 4, p.15 of the datasheet.
 */
#define BYTES_PER_PATTERN 8

/**
 * The total number of bytes the file has for the characters' pattern.
 */
#define CHARACTER_PATTERN_SIZE (NUM_CHARACTER_CODES * BYTES_PER_CHARACTER)

#ifdef LCDSIM_20x4
/**
 * The number of characters the LCD displays at once.
 * Assuming a LCD of 20x4, the total is 80 characters.
 */
#define NUM_CHARS_LCD 80

/**
 * The number of characters per line.
 * Assuming a LCD 20x4, the total is 20 characters.
 */
#define CHARS_PER_LINE 20
#else
/**
 * The number of characters the LCD displays at once.
 * Assuming a LCD of 16x2, the total is 32 characters.
 */
#define NUM_CHARS_LCD 32

/**
 * The number of characters per line.
 * Assuming a LCD 16x2, the total is 16 characters.
 */
#define CHARS_PER_LINE 16
#endif

#define FIRST_LINE_ADDRESS  0x00

/**
 * The address refers to the DDRAM address wich corresponds to the beginning of the second
 * line on the LCD. See p.11 of the datasheet.
 */
#define SECOND_LINE_ADDRESS 0x40

#ifdef LCDSIM_20x4

#define THIRD_LINE_ADDRESS  0x14
#define FOURTH_LINE_ADDRESS 0x54

#endif

/**
 * The address refers to the DDRAM address which corresponds to the last position of the first line on the LCD.
 * See p.11 of the datasheet.
 */
#define FIRST_LINE_LAST_POS_DDRAM_ADDR 0x27

/**
 * The address refers to the DDRAM address which corresponds to the last position of the second line on the LCD.
 * See p.11 of the datasheet.
 */
#define SECOND_LINE_LAST_POS_DDRAM_ADDR 0x67

/**
 * The DDRAM address refers to the positio on the display. See p.11 of the datasheet.
 */
#define SET_DDRAM_ADDRESS 0x80

/**
 * The CGRAM address refers to custom characters defined by the user. See p.13 of the datasheet. 
 */
#define SET_CGRAM_ADDRESS 0x40

/**
 * The margin in the X axis between the LCD image and the pixels of the characters.
 * The margin in the X axis between the LCD image and the pixels of the characters.
 */
#ifdef LCDSIM_20x4
    #define MARGIN_LCD_X 40
    #define MARGIN_LCD_Y 50
#else
    #define MARGIN_LCD_X 38
    #define MARGIN_LCD_Y 50
#endif

/**
 * The modified pixel size of each pixel on the LCD. 
 */
#define PIXEL_SIZE 3

/* Values for the LCD pixel colors */
typedef enum Color Color;
enum Color
{
    GREEN = 0,
    BLACK = 1,
    COLORS = 2
};

typedef enum Cursor Cursor;
enum Cursor
{
    FIXED = 0,
    BLINK = 1
};

enum Display_Ctrl
{
    DISPLAY_OFF = 0,
    DISPLAY_ON = 1
};

/* Values to identify to which RAM we are going to write in */
typedef enum Actual Actual;
enum Actual
{
    CGR = 0,
    DDR = 1
};

#endif /* LCDSIM_DEFS_H */