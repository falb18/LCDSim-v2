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
void LCDSim_Draw(LCDSim *lcdsim);
void LCDSim_Instruction(LCDSim *lcdsim, Uint16 instruction);
LCDSim* LCDSim_Destroy(LCDSim *lcdsim);

#endif /* LCDSIM_H */