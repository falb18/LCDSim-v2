#ifndef LCDSIM_H
#define LCDSIM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "lcdsim-defs/lcdsim-defs.h"
#include "hd44780/hd44780.h"

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