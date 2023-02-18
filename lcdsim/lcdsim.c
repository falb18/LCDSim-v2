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
    LCDSim* lcdsim = malloc(sizeof(LCDSim));

    if (lcdsim != NULL)
    {
        lcdsim->gu.screen = sdl_screen;
        HD44780_Init(&lcdsim->mcu);
        GraphicUnit_Init(&lcdsim->gu);
        lcdsim->lastTime = SDL_GetTicks();
    }
    
    return lcdsim;
}

void LCDSim_Draw(LCDSim *lcdsim)
{
    Uint32 nowTime = SDL_GetTicks();
    
    /* Copy image to LCD's screen buffer */
    SDL_RenderCopy(lcdsim->gu.screen, lcdsim->gu.image, NULL, NULL);

    if (nowTime - lcdsim->lastTime > 500)
    {
        lcdsim->mcu.LCD_CursorState = !lcdsim->mcu.LCD_CursorState;
        lcdsim->lastTime = nowTime;
    }

    Pixel_Refresh(lcdsim->mcu, lcdsim->gu.pixel);
    Pixel_Draw(&lcdsim->gu);
}

void LCDSim_Instruction(LCDSim *lcdsim, Uint16 instruction)
{

    Uint8 i, n, m;
    if (instruction & 0x0100)
    {
        if (lcdsim->mcu.RAM_current == CGR)
        {
            n = lcdsim->mcu.CGRAM_counter / 8;
            m = lcdsim->mcu.CGRAM_counter % 8;
            lcdsim->mcu.CGROM[n][m] = instruction & 0xFF;
            if (lcdsim->mcu.CGRAM_counter < 64)
                lcdsim->mcu.CGRAM_counter++;
        }
        else
        {
            lcdsim->mcu.DDRAM[lcdsim->mcu.DDRAM_counter] = instruction & 0xFF;
            if (lcdsim->mcu.LCD_EntryMode & 0x02)
            {
                if (lcdsim->mcu.DDRAM_counter < (SECOND_LINE_LAST_POS_DDRAM_ADDR + 1))
                {
                    if (lcdsim->mcu.DDRAM_counter == FIRST_LINE_LAST_POS_DDRAM_ADDR)
                        lcdsim->mcu.DDRAM_counter = SECOND_LINE_ADDRESS;
                    else
                        lcdsim->mcu.DDRAM_counter++;
                }
                if (lcdsim->mcu.LCD_EntryMode & 0x01)
                {
                    if (lcdsim->mcu.DDRAM_display < 24)
                        lcdsim->mcu.DDRAM_display++;
                }
            }
            else
            {
                if (lcdsim->mcu.DDRAM_counter > 0)
                {
                    if (lcdsim->mcu.DDRAM_counter == SECOND_LINE_ADDRESS)
                        lcdsim->mcu.DDRAM_counter = FIRST_LINE_LAST_POS_DDRAM_ADDR;
                    else
                        lcdsim->mcu.DDRAM_counter--;
                }
                if (lcdsim->mcu.LCD_EntryMode & 0x01)
                {
                    if (lcdsim->mcu.DDRAM_display > 0)
                        lcdsim->mcu.DDRAM_display--;
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
                lcdsim->mcu.DDRAM_counter = instruction & 0x7F;
                lcdsim->mcu.RAM_current = DDR;
                break;
            /* SET CGRAM ADDRESS */
            case 1:
                lcdsim->mcu.CGRAM_counter = instruction & 0x3F;
                lcdsim->mcu.RAM_current = CGR;
                break;
            /* CURSOR/DISPLAY SHIFT */
            case 3:
                if (instruction & 0x08)
                {
                    if (instruction & 0x04)
                    {
                        if (lcdsim->mcu.DDRAM_display < 24)
                            lcdsim->mcu.DDRAM_display++;
                    }
                    else
                    {
                        if (lcdsim->mcu.DDRAM_display > 0)
                            lcdsim->mcu.DDRAM_display--;
                    }
                }
                else
                {
                    if (instruction & 0x04)
                    {
                        if (lcdsim->mcu.DDRAM_counter < (SECOND_LINE_LAST_POS_DDRAM_ADDR + 1))
                        {
                            if (lcdsim->mcu.DDRAM_counter == FIRST_LINE_LAST_POS_DDRAM_ADDR)
                                lcdsim->mcu.DDRAM_counter = SECOND_LINE_ADDRESS;
                            else
                                lcdsim->mcu.DDRAM_counter++;
                        }
                    }
                    else
                    {
                        if (lcdsim->mcu.DDRAM_counter > 0)
                        {
                            if (lcdsim->mcu.DDRAM_counter == SECOND_LINE_ADDRESS)
                                lcdsim->mcu.DDRAM_counter = FIRST_LINE_LAST_POS_DDRAM_ADDR;
                            else
                                lcdsim->mcu.DDRAM_counter--;
                        }
                    }
                }
                break;
            /* DISPLAY ON/OFF CONTROL */
            case 4:
                lcdsim->mcu.LCD_CursorBlink = instruction & 0x01;
                lcdsim->mcu.LCD_CursorEnable = (instruction & 0x02) >> 1;
                lcdsim->mcu.LCD_DisplayEnable = (instruction & 0x04) >> 2;
                lcdsim->mcu.LCD_CursorState = 0;
                break;
            /* ENTRY MODE SET */
            case 5:
                lcdsim->mcu.LCD_EntryMode = instruction & 0x03;
                break;
            /* HOME */
            case 6:
                lcdsim->mcu.DDRAM_counter = 0;
                lcdsim->mcu.DDRAM_display = 0;
                break;
            /* CLEAR */
            case 7:
                for (i = 0; i < 80; i++)
                    lcdsim->mcu.DDRAM[i] = 0x20;
                lcdsim->mcu.DDRAM_counter = 0;
                lcdsim->mcu.DDRAM_display = 0;
                break;
        }
    }
}

LCDSim* LCDSim_Destroy(LCDSim *lcdsim)
{
    SDL_DestroyTexture(lcdsim->gu.image);
    SDL_DestroyTexture(lcdsim->gu.color[0]);
    SDL_DestroyTexture(lcdsim->gu.color[1]);
    SDL_DestroyRenderer(sdl_screen);
    SDL_DestroyWindow(sdl_window);
    free(lcdsim);
    return NULL;
}

void LCD_PutChar(LCDSim *lcdsim, char car)
{
    LCDSim_Instruction(lcdsim, 0x0100 | car);
}

void LCD_PutS(LCDSim *lcdsim, char *s)
{
    while (*s)
        LCD_PutChar(lcdsim, *s++);
}

void LCD_Clear(LCDSim *lcdsim)
{
    LCDSim_Instruction(lcdsim, CLEAR_DISPLAY);
}

void LCD_Home(LCDSim *lcdsim)
{
    LCDSim_Instruction(lcdsim, LCD_HOME);
}

void LCD_State(LCDSim *lcdsim, Uint8 display_enable, Uint8 cursor_enable, Uint8 blink)
{
    LCDSim_Instruction(lcdsim, 0x08 | (display_enable << 2) | (cursor_enable << 1) | blink);
}

void LCD_Sh_Cursor_R(LCDSim *lcdsim)
{
    LCDSim_Instruction(lcdsim, SHIFT_CURSOR_RIGHT);
}

void LCD_Sh_Cursor_L(LCDSim *lcdsim)
{
   LCDSim_Instruction(lcdsim, SHIFT_CURSOR_LEFT);
}

void LCD_Sh_Display_R(LCDSim *lcdsim)
{
   LCDSim_Instruction(lcdsim, SHIFT_DISPLAY_RIGHT);
}

void LCD_Sh_Display_L(LCDSim *lcdsim)
{
    LCDSim_Instruction(lcdsim, SHIFT_DISPLAY_LEFT);
}

void LCD_ClearLine(LCDSim *lcdsim, Uint8 line)
{
    Uint8 i;
    
    if ((line == 0) || (line == 1))
    {
        LCD_SetCursor(lcdsim, line, 0);
        
        for (i = 0; i < 40; i++)
        {
            LCD_PutChar(lcdsim, ' ');
        }
        
        LCD_SetCursor(lcdsim, line, 0);
    }
}

void LCD_SetCursor(LCDSim *lcdsim, Uint8 line, Uint8 column)
{
    Uint8 pos;

    if ((column > 15) || (line > 1))
    {
        return;
    }
    
    pos = (line * SECOND_LINE_ADDRESS) + column;
    LCDSim_Instruction(lcdsim, SET_DDRAM_ADDRESS | pos);
}

void LCD_CustomChar(LCDSim *lcdsim, Uint8 char_number, Uint8* custom)
{
    Uint8 i;

    if(char_number < 0 && char_number > 7)
    {
        return;
    }

    LCDSim_Instruction(lcdsim, SET_CGRAM_ADDRESS | char_number * 0x08);
    
    for(i = 0; i < 8; i++)
    {
        LCD_PutChar(lcdsim, custom[i]);
    }

    LCDSim_Instruction(lcdsim, SET_DDRAM_ADDRESS);
}