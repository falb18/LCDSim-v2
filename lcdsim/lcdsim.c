#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "lcdsim.h"

LCDSim* LCDSim_Create(SDL_Renderer *screen, int x, int y)
{

    LCDSim* self = malloc(sizeof(LCDSim));

    if (self != NULL)
    {
        self->gu.screen = screen;
        self->gu.on_screen.x = x;
        self->gu.on_screen.y = y;
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
}

void HD44780_Init(HD44780 *self)
{

    /* Init internal registers of the HD44780 hardware emulator */
    self->CGRAM_counter = 0;
    self->DDRAM_counter = 0;
    self->DDRAM_display = 0;
    self->LCD_EntryMode = 0x02;
    self->LCD_CursorEnable = 0;
    self->LCD_CursorBlink = FIXED;
    self->LCD_CursorState = 0;
    self->LCD_DisplayEnable = 1;
    self->RAM_current = DDR;

    /* Init CGROM */
    Uint16 i;
    Uint8 j, cgrom_array[1152];
    FILE *cgrom = fopen("../res/cgrom.bin", "rb");
    fread(cgrom_array, sizeof(cgrom_array), 1, cgrom);

    for (i = 0; i < 1152; i += 9)
    {
        if (cgrom_array[i] != 0)
        {
            for (j = 0; j < 8; j++)
            {
                self->CGROM[cgrom_array[i]][j] = cgrom_array[i+j+1];
            }
        }
        else
        { 
            break;
        }
    }

    fclose(cgrom);

    /* Initialize DDRAM with blank character (character code = 0x20) */
    memset(self->DDRAM, 0x20, 104);
}

void GraphicUnit_Init(GraphicUnit *self)
{
    Uint8 i;
    SDL_Surface *colors_surface[COLORS];

    self->position.x = 0;
    self->position.y = 0;

    /* Create the tiny pixels of the LCD: */
    for (i = 0; i < COLORS; i++)
    {
        colors_surface[i] = SDL_CreateRGBSurface(0, PIXEL_DIM, PIXEL_DIM, 32, 0, 0, 0, 0);
    }

    SDL_FillRect(colors_surface[BLACK], NULL, SDL_MapRGB(colors_surface[BLACK]->format, 0, 0, 0));
    SDL_FillRect(colors_surface[GREEN], NULL, SDL_MapRGB(colors_surface[GREEN]->format, 125, 159, 50));

    self->color[BLACK] = SDL_CreateTextureFromSurface(self->screen, colors_surface[BLACK]);
    self->color[GREEN] = SDL_CreateTextureFromSurface(self->screen, colors_surface[GREEN]);

    self->temp_screen = SDL_CreateRGBSurface(0, 331, 149, 32, 0, 0, 0, 0);

    /* Free the surfaces since are not going to be used anymore */
    free(colors_surface[BLACK]);
    free(colors_surface[GREEN]);

    /* Load the image for the LCD: */
    self->image = IMG_LoadTexture(self->screen, "../res/lcd_layout.bmp");
    
    Pixel_Init(self->pixel);
}

void Pixel_Init(Pixel pixel[][CASE_WIDTH][CASE_HEIGHT])
{
    Uint8 z, x , y;

    for (z = 0; z < 32; z++)
    {
        for(x = 0 ; x < CASE_WIDTH; x++)
        {
            for(y = 0 ; y < CASE_HEIGHT; y++)
            {
                pixel[z][x][y].position.x = OFFSET_X + (z % 16) * 16 + x*PIXEL_DIM;
                pixel[z][x][y].position.y = OFFSET_Y + (z >= 16) * 25 + y*PIXEL_DIM;
                pixel[z][x][y].color = GREEN;
            }
        }
    }
}