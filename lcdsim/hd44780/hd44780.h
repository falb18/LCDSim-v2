#ifndef HD44780_H
#define HD44780_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../lcdsim-defs/lcdsim-defs.h"

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
    Pixel pixel[32][LCD_FONT_WIDTH][LCD_FONT_HEIGHT];
    SDL_Rect position;
    SDL_Renderer *screen;
    SDL_Texture *image;
    SDL_Texture *color[2];
} GraphicUnit;

/* Functions related to the HD44780 hardware emulation: */

void HD44780_Init(HD44780 *self);
void GraphicUnit_Init(GraphicUnit *self);
void Pixel_Init(Pixel pixel[][LCD_FONT_WIDTH][LCD_FONT_HEIGHT]);
void Pixel_Refresh(HD44780 mcu, Pixel pixel[][LCD_FONT_WIDTH][LCD_FONT_HEIGHT]);
void Pixel_Draw(GraphicUnit *self);

#endif /* HD44780_H */