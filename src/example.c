#include "lcdsim.h"

int main (int argc, char** argv)
{
    SDL_Event event;
    SDL_Window *sdl_window = NULL;
    SDL_Renderer *sdl_screen = NULL;
    SDL_Texture *lcd_image = NULL;
    LCDSim *lcd = NULL;
    Uint8 hold = 1;

    /* Initialization of the SDL */
    SDL_Init(SDL_INIT_VIDEO);
    sdl_window = SDL_CreateWindow("LCDSim 16x2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 331, 149, 0);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    sdl_screen = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);

    SDL_UpdateWindowSurface(sdl_window);

    /* Initialize the LCD object */
    lcd = LCDSim_Create(sdl_screen, 0, 0);

    LCD_State(lcd, 1, 1, 1);
    LCD_SetCursor(lcd, 0, 3);
    LCD_PutS(lcd, "Hello,");
    LCD_SetCursor(lcd, 1, 5);
    LCD_PutS(lcd, "GitHub!");

    /* Run the program until the close button hasn't been pressed */
    while (hold) {
        SDL_PollEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                hold = 0;
                break;
            default:
                break;
        }

        /* Draw the LCD on window */
        LCDSim_Draw(lcd);

        /* Refresh the window at 50 FPS */
        SDL_RenderPresent(sdl_screen);
        SDL_Delay(20);
    }

    lcd = LCDSim_Destroy(lcd);
    SDL_DestroyRenderer(sdl_screen);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
}