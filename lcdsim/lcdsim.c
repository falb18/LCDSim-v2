#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "lcdsim.h"

#ifdef LCDSIM_20x4
    #define WINDOW_WIDTH 400
    #define WINDOW_HEIGHT 199
#else
    #define WINDOW_WIDTH 331
    #define WINDOW_HEIGHT 149
#endif

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
static void lcdsim_draw_pixels(Uint8 *pixels);

/*
 * External functions:
 * ==============================================
 */

LCDSim* LCDSim_Init()
{
    char window_title[30];

    sprintf(window_title, "LCDSim v%s", LCDSIM_VERSION);

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
    #ifdef LCDSIM_20x4
        graph_unit->image = IMG_LoadTexture(graph_unit->screen, LCDSIM_20x4_LCD_GREEN);
    #else
        #ifdef LCDSIM_BLUE
            graph_unit->image = IMG_LoadTexture(graph_unit->screen, LCDSIM_16x2_LCD_BLUE);
        #else
            graph_unit->image = IMG_LoadTexture(graph_unit->screen, LCDSIM_16x2_LCD_GREEN);
        #endif
    #endif

    if (graph_unit->image == NULL)
    {
        printf("Image not found\n");
    }
}

static void lcdsim_draw_pixels(Uint8 *pixels)
{
    Uint8 char_idx = 0, x = 0, y = 0;
    Uint8 start_x = LCD_FONT_WIDTH * PIXEL_SIZE + 1;
    Uint8 start_y = LCD_FONT_HEIGHT * PIXEL_SIZE + 1;
    Uint8 row = 0;
    Uint16 idx = 0;
    SDL_Rect pixel;

    pixel.w = PIXEL_SIZE;
    pixel.h = PIXEL_SIZE;

    for (char_idx = 0; char_idx < NUM_CHARS_LCD; char_idx++)
    {
        row = (uint8_t)(char_idx / CHARS_PER_LINE);

        for (x = 0; x < LCD_FONT_WIDTH; x++)
        {
            for (y = 0; y < LCD_FONT_HEIGHT; y++)
            {
                idx = y + (x * LCD_FONT_HEIGHT) + (char_idx * LCD_FONT_WIDTH * LCD_FONT_HEIGHT);
                if (pixels[idx] == BLACK)
                {
                    #ifdef LCDSIM_BLUE
                        SDL_SetRenderDrawColor(sdl_screen, 213, 224 , 247, SDL_ALPHA_OPAQUE);
                    #else
                        SDL_SetRenderDrawColor(sdl_screen, 44, 47, 38, SDL_ALPHA_OPAQUE);
                    #endif
                }
                else
                {
                    #ifdef LCDSIM_BLUE
                       SDL_SetRenderDrawColor(sdl_screen, 59, 103, 189, SDL_ALPHA_OPAQUE);
                    #else
                        SDL_SetRenderDrawColor(sdl_screen, 125, 159, 50, SDL_ALPHA_OPAQUE);
                    #endif
                }

                /* The number 16 = 5 pixels of font width * PIXEL_SIZE + 1 pixel of space between characters
                 * The number 25 = 8 pixels of font height * PIXEL_SIZE + 1 pixel of space between characters
                 */
                pixel.x = MARGIN_LCD_X + ((char_idx % CHARS_PER_LINE) * start_x) + (x * PIXEL_SIZE);
                pixel.y = MARGIN_LCD_Y + (row * start_y) + (y * PIXEL_SIZE);

                SDL_RenderFillRect(sdl_screen, &pixel);
            }
        }
    }
}