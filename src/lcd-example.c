#include "lcdsim.h"

int main (int argc, char** argv)
{
    SDL_Event event;
    LCDSim *lcd = NULL;
    Uint8 hold = 1;

    /* Initialize the LCD object */
    lcd = LCDSim_Init();

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
        SDL_RenderPresent(lcd->gu.screen);
        SDL_Delay(20);
    }

    lcd = LCDSim_Destroy(lcd);
    SDL_Quit();
}