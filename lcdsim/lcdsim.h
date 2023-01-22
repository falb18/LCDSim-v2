#ifndef LCDSIM_H
#define LCDSIM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define CLEAR_DISPLAY        0x01
#define LCD_HOME             0x02
#define LCD_START            0x03
#define ENTRY_MODE_SET       0x06
#define DISPLAY              0x08
#define SHIFT_CURSOR_LEFT    0x10
#define SHIFT_CURSOR_RIGHT   0x14
#define SHIFT_DISPLAY_LEFT   0x18
#define SHIFT_DISPLAY_RIGHT  0x1C
#define FUNCTION_SET         0x28
#define SET_DDRAM_AD         0x80
#define SET_CGRAM_AD         0x40
#define CASE_WIDTH 5
#define CASE_HEIGHT 8
#define OFFSET_X 38
#define OFFSET_Y 50
#define PIXEL_DIM 3

/* Values for the LCD pixel colors */
typedef enum Color Color;
enum Color { BLACK = 0, GREEN = 1};

typedef enum Cursor Cursor;
enum Cursor { FIXED = 0, BLINK = 1};

/* Values to identify to which RAM we are going to write in */
typedef enum Actual Actual;
enum Actual { CGR = 0, DDR = 1};


typedef struct {
    SDL_Rect position;
    Color color;
} Pixel;

typedef struct {
    Actual RAM_current;
    Uint8 DDRAM[104];
    Uint8 CGROM[128][8];
    Uint8 DDRAM_counter;
    Uint8 CGRAM_counter;
    Uint8 DDRAM_display;
    Uint8 LCD_EntryMode;
    Uint8 LCD_DisplayEnable;
    Uint8 LCD_CursorEnable;
    Uint8 LCD_CursorState;
    Cursor LCD_CursorBlink;
} HD44780;

typedef struct {
    Pixel pixel[32][CASE_WIDTH][CASE_HEIGHT];
    SDL_Rect position, on_screen;
    SDL_Renderer *screen;
    SDL_Surface *temp_screen;
    SDL_Texture *image;
    SDL_Surface *color[2];
} GraphicUnit;

typedef struct {
    HD44780 mcu;
    GraphicUnit gu;
    Uint32 lastTime;
} LCDSim;

/* Functions related to the LCD simulator: */

LCDSim* LCDSim_Create(SDL_Renderer *screen, int x, int y);

/* Functions related to the HD44780 hardware emulation: */

void HD44780_Init(HD44780 *self);
void GraphicUnit_Init(GraphicUnit *self);
void Pixel_Init(Pixel pixel[][CASE_WIDTH][CASE_HEIGHT]);

#endif /* LCDSIM_H */