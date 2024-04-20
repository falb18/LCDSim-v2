#ifndef HD44780_H
#define HD44780_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../lcdsim-defs/lcdsim-defs.h"

typedef struct {
    Actual RAM_current;
    Uint8 DDRAM[104];
    Uint8 CGROM[NUM_CHARACTER_CODES][8];
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
    Uint8 *lcd_pixels;
    SDL_Renderer *screen;
    SDL_Texture *image;
} GraphicUnit;

/* Functions related to the HD44780 hardware emulation: */

void HD44780_Init(HD44780 *mcu, GraphicUnit *graph_unit);
void HD44780_Update(HD44780 *mcu, GraphicUnit *graph_unit);
void HD44780_ParseCMD(HD44780 *mcu, Uint16 instruction);

#endif /* HD44780_H */