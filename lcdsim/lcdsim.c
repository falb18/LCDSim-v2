#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "lcdsim.h"

#define WINDOW_WIDTH 331
#define WINDOW_HEIGHT 149

/*
 * Private variables
 * ==============================================
 */

SDL_Window *sdl_window = NULL;
SDL_Renderer *sdl_screen = NULL;

/*
 * External functions:
 * ==============================================
 */

LCDSim* LCDSim_Init()
{
    /* Initialize SDL window and screen: */
    SDL_Init(SDL_INIT_VIDEO);
    sdl_window = SDL_CreateWindow("LCDSim 16x2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                    WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    sdl_screen = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);

    SDL_UpdateWindowSurface(sdl_window);

    /* Create LCD display: */
    LCDSim* self = malloc(sizeof(LCDSim));

    if (self != NULL)
    {
        self->gu.screen = sdl_screen;
        HD44780_Init(&self->mcu);
        GraphicUnit_Init(&self->gu);
        self->lastTime = SDL_GetTicks();
    }
    
    return self;
}

void LCDSim_Draw(LCDSim *self)
{
    Uint32 nowTime = SDL_GetTicks();
    
    /* Copy image to LCD's screen buffer */
    SDL_RenderCopy(self->gu.screen, self->gu.image, NULL, NULL);

    if (nowTime - self->lastTime > 500)
    {
        self->mcu.LCD_CursorState = !self->mcu.LCD_CursorState;
        self->lastTime = nowTime;
    }

    Pixel_Refresh(self->mcu, self->gu.pixel);
    Pixel_Draw(&self->gu);
}

void LCDSim_Instruction(LCDSim *self, Uint16 instruction)
{

    Uint8 i, n, m;
    if (instruction & 0x0100)
    {
        if (self->mcu.RAM_current == CGR)
        {
            n = self->mcu.CGRAM_counter / 8;
            m = self->mcu.CGRAM_counter % 8;
            self->mcu.CGROM[n][m] = instruction & 0xFF;
            if (self->mcu.CGRAM_counter < 64)
                self->mcu.CGRAM_counter++;
        }
        else
        {
            self->mcu.DDRAM[self->mcu.DDRAM_counter] = instruction & 0xFF;
            if (self->mcu.LCD_EntryMode & 0x02)
            {
                if (self->mcu.DDRAM_counter < (SECOND_LINE_LAST_POS_DDRAM_ADDR + 1))
                {
                    if (self->mcu.DDRAM_counter == FIRST_LINE_LAST_POS_DDRAM_ADDR)
                        self->mcu.DDRAM_counter = SECOND_LINE_ADDRESS;
                    else
                        self->mcu.DDRAM_counter++;
                }
                if (self->mcu.LCD_EntryMode & 0x01)
                {
                    if (self->mcu.DDRAM_display < 24)
                        self->mcu.DDRAM_display++;
                }
            }
            else
            {
                if (self->mcu.DDRAM_counter > 0)
                {
                    if (self->mcu.DDRAM_counter == SECOND_LINE_ADDRESS)
                        self->mcu.DDRAM_counter = FIRST_LINE_LAST_POS_DDRAM_ADDR;
                    else
                        self->mcu.DDRAM_counter--;
                }
                if (self->mcu.LCD_EntryMode & 0x01)
                {
                    if (self->mcu.DDRAM_display > 0)
                        self->mcu.DDRAM_display--;
                }
            }
        }
    }
    else
    {
        for (i = 0; i < 8; i++)
            if (instruction & (0x80 >> i))
                break;
        switch (i)
        {
            /* SET DDRAM ADDRESS */
            case 0:
                self->mcu.DDRAM_counter = instruction & 0x7F;
                self->mcu.RAM_current = DDR;
                break;
            /* SET CGRAM ADDRESS */
            case 1:
                self->mcu.CGRAM_counter = instruction & 0x3F;
                self->mcu.RAM_current = CGR;
                break;
            /* CURSOR/DISPLAY SHIFT */
            case 3:
                if (instruction & 0x08)
                {
                    if (instruction & 0x04)
                    {
                        if (self->mcu.DDRAM_display < 24)
                            self->mcu.DDRAM_display++;
                    }
                    else
                    {
                        if (self->mcu.DDRAM_display > 0)
                            self->mcu.DDRAM_display--;
                    }
                }
                else
                {
                    if (instruction & 0x04)
                    {
                        if (self->mcu.DDRAM_counter < (SECOND_LINE_LAST_POS_DDRAM_ADDR + 1))
                        {
                            if (self->mcu.DDRAM_counter == FIRST_LINE_LAST_POS_DDRAM_ADDR)
                                self->mcu.DDRAM_counter = SECOND_LINE_ADDRESS;
                            else
                                self->mcu.DDRAM_counter++;
                        }
                    }
                    else
                    {
                        if (self->mcu.DDRAM_counter > 0)
                        {
                            if (self->mcu.DDRAM_counter == SECOND_LINE_ADDRESS)
                                self->mcu.DDRAM_counter = FIRST_LINE_LAST_POS_DDRAM_ADDR;
                            else
                                self->mcu.DDRAM_counter--;
                        }
                    }
                }
                break;
            /* DISPLAY ON/OFF CONTROL */
            case 4:
                self->mcu.LCD_CursorBlink = instruction & 0x01;
                self->mcu.LCD_CursorEnable = (instruction & 0x02) >> 1;
                self->mcu.LCD_DisplayEnable = (instruction & 0x04) >> 2;
                self->mcu.LCD_CursorState = 0;
                break;
            /* ENTRY MODE SET */
            case 5:
                self->mcu.LCD_EntryMode = instruction & 0x03;
                break;
            /* HOME */
            case 6:
                self->mcu.DDRAM_counter = 0;
                self->mcu.DDRAM_display = 0;
                break;
            /* CLEAR */
            case 7:
                for (i = 0; i < 80; i++)
                    self->mcu.DDRAM[i] = 0x20;
                self->mcu.DDRAM_counter = 0;
                self->mcu.DDRAM_display = 0;
                break;
        }
    }
}

LCDSim* LCDSim_Destroy(LCDSim *self)
{
    SDL_DestroyTexture(self->gu.image);
    SDL_DestroyTexture(self->gu.color[0]);
    SDL_DestroyTexture(self->gu.color[1]);
    SDL_DestroyRenderer(sdl_screen);
    SDL_DestroyWindow(sdl_window);
    free(self);
    return NULL;
}

void LCD_PutChar(LCDSim *self, char car)
{
    LCDSim_Instruction(self, 0x0100 | car);
}

void LCD_PutS(LCDSim *self, char *s)
{
    while (*s)
        LCD_PutChar(self, *s++);
}

void LCD_Clear(LCDSim *self)
{
    LCDSim_Instruction(self, CLEAR_DISPLAY);
}

void LCD_Home(LCDSim *self)
{
    LCDSim_Instruction(self, LCD_HOME);
}

void LCD_State(LCDSim *self, Uint8 display_enable, Uint8 cursor_enable, Uint8 blink)
{
    LCDSim_Instruction(self, 0x08 | (display_enable << 2) | (cursor_enable << 1) | blink);
}

void LCD_Sh_Cursor_R(LCDSim *self)
{
    LCDSim_Instruction(self, SHIFT_CURSOR_RIGHT);
}

void LCD_Sh_Cursor_L(LCDSim *self)
{
   LCDSim_Instruction(self, SHIFT_CURSOR_LEFT);
}

void LCD_Sh_Display_R(LCDSim *self)
{
   LCDSim_Instruction(self, SHIFT_DISPLAY_RIGHT);
}

void LCD_Sh_Display_L(LCDSim *self)
{
    LCDSim_Instruction(self, SHIFT_DISPLAY_LEFT);
}

void LCD_ClearLine(LCDSim *self, Uint8 line)
{
    Uint8 i;
    
    if ((line == 0) || (line == 1))
    {
        LCD_SetCursor(self, line, 0);
        
        for (i = 0; i < 40; i++)
        {
            LCD_PutChar(self, ' ');
        }
        
        LCD_SetCursor(self, line, 0);
    }
}

void LCD_SetCursor(LCDSim *self, Uint8 line, Uint8 column)
{
    Uint8 pos;

    if ((column > 15) || (line > 1))
    {
        return;
    }
    
    pos = (line * SECOND_LINE_ADDRESS) + column;
    LCDSim_Instruction(self, SET_DDRAM_ADDRESS | pos);
}

void LCD_CustomChar(LCDSim *self, Uint8 char_number, Uint8* custom)
{
    Uint8 i;

    if(char_number < 0 && char_number > 7)
    {
        return;
    }

    LCDSim_Instruction(self, SET_CGRAM_ADDRESS | char_number * 0x08);
    
    for(i = 0; i < 8; i++)
    {
        LCD_PutChar(self, custom[i]);
    }

    LCDSim_Instruction(self, SET_DDRAM_ADDRESS);
}