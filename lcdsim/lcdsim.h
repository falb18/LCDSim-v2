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

/* Values for the LCD pixel colors */
typedef enum Color Color;
enum Color { BLACK = 0, GREEN = 1, COLORS = 2};

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
    Pixel pixel[32][LCD_FONT_WIDTH][LCD_FONT_HEIGHT];
    SDL_Rect position;
    SDL_Renderer *screen;
    SDL_Texture *image;
    SDL_Texture *color[2];
} GraphicUnit;

typedef struct {
    HD44780 mcu;
    GraphicUnit gu;
    Uint32 lastTime;
} LCDSim;

/* Functions related to the LCD emulator: */

LCDSim* LCDSim_Init();
void LCDSim_Draw(LCDSim *self);
void LCDSim_Instruction(LCDSim *self, Uint16 instruction);
LCDSim* LCDSim_Destroy(LCDSim *self);

/* Functions related to the HD44780 hardware emulation: */

void HD44780_Init(HD44780 *self);
void GraphicUnit_Init(GraphicUnit *self);
void Pixel_Init(Pixel pixel[][LCD_FONT_WIDTH][LCD_FONT_HEIGHT]);
void Pixel_Refresh(HD44780 mcu, Pixel pixel[][LCD_FONT_WIDTH][LCD_FONT_HEIGHT]);
void Pixel_Draw(GraphicUnit *self);

/* Functions to write on the LCD: */

void LCD_PutChar(LCDSim *self, char car);
void LCD_PutS(LCDSim *self, char *s);
void LCD_State(LCDSim *self, Uint8 display_enable, Uint8 cursor_enable, Uint8 blink);
void LCD_Clear(LCDSim *self);
void LCD_Home(LCDSim *self);
void LCD_SetCursor(LCDSim *self, Uint8 line, Uint8 column);
void LCD_Sh_Cursor_R(LCDSim *self);
void LCD_Sh_Cursor_L(LCDSim *self);
void LCD_Sh_Display_R(LCDSim *self);
void LCD_Sh_Display_L(LCDSim *self);
void LCD_ClearLine(LCDSim *self, Uint8 line);
void LCD_SetCursor(LCDSim *self, Uint8 line, Uint8 column);
void LCD_CustomChar(LCDSim *self, Uint8 char_number, Uint8* custom);

#endif /* LCDSIM_H */