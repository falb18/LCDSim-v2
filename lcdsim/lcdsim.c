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
 * Private function protoypes:
 * ==============================================
 */

static void lcdsim_load_image(GraphicUnit *graph_unit);
static void lcdsim_draw_pixels(Uint8 pixels[][LCD_FONT_WIDTH][LCD_FONT_HEIGHT]);

/*
 * External functions:
 * ==============================================
 */

LCDSim* LCDSim_Init()
{
    char window_title[30];

    sprintf(window_title, "LCDSim 16x2 v%s", LCDSIM_VERSION);

    /* Initialize SDL window and screen: */
    SDL_Init(SDL_INIT_VIDEO);
    sdl_window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                    WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    sdl_screen = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);

    SDL_UpdateWindowSurface(sdl_window);

    /* Create LCD display: */
    LCDSim* lcdsim = malloc(sizeof(LCDSim));

    if (lcdsim != NULL)
    {
        lcdsim->gu.screen = sdl_screen;
        HD44780_Init(&lcdsim->mcu, &lcdsim->gu);
        lcdsim_load_image(&lcdsim->gu);
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

    HD44780_Update(lcdsim->mcu, &lcdsim->gu);
    lcdsim_draw_pixels(lcdsim->gu.lcd_pixels);
}

void LCDSim_Instruction(LCDSim *lcdsim, Uint16 instruction)
{
    HD44780_ParseCMD(&lcdsim->mcu, instruction);
}

LCDSim* LCDSim_Destroy(LCDSim *lcdsim)
{
    SDL_DestroyTexture(lcdsim->gu.image);
    SDL_DestroyRenderer(sdl_screen);
    SDL_DestroyWindow(sdl_window);
    free(lcdsim);
    return NULL;
}

static void lcdsim_load_image(GraphicUnit *graph_unit)
{
    graph_unit->image = IMG_LoadTexture(graph_unit->screen, "../res/lcd_layout.bmp");
}

static void lcdsim_draw_pixels(Uint8 pixels[][LCD_FONT_WIDTH][LCD_FONT_HEIGHT])
{
    Uint8 char_idx, x, y;
    Uint8 pixel_color;
    SDL_Rect pixel;

    pixel.w = PIXEL_SIZE;
    pixel.h = PIXEL_SIZE;

    for (char_idx = 0; char_idx < NUM_CHARS_LCD; char_idx++)
    {
        for (x = 0; x < LCD_FONT_WIDTH; x++)
        {
            for (y = 0; y < LCD_FONT_HEIGHT; y++)
            {
                if (pixels[char_idx][x][y] == BLACK)
                {
                    SDL_SetRenderDrawColor(sdl_screen, 0, 0, 0, SDL_ALPHA_OPAQUE);
                }
                else
                {
                    SDL_SetRenderDrawColor(sdl_screen, 125, 159, 50, SDL_ALPHA_OPAQUE);
                }
                
                pixel.x = MARGIN_LCD_X + ((char_idx % CHARS_PER_LINE) * 16) + (x * PIXEL_SIZE);
                pixel.y = MARGIN_LCD_Y + ((char_idx >= CHARS_PER_LINE) * 25) + (y * PIXEL_SIZE);

                SDL_RenderFillRect(sdl_screen, &pixel);
            }
        }
    }
}