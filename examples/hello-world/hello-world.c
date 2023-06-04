#include "lcdsim.h"
#include "avr-hd44780.h"

int main (int argc, char** argv)
{
    SDL_Event event;
    LCDSim *lcd = NULL;
    Uint8 hold = 1;

    /* Initialize the LCD object */
    lcd = LCDSim_Init();

    lcd_init_lib(lcd);
    lcd_on();

    lcd_set_cursor(3, 0);
    lcd_puts("Hello");
    lcd_set_cursor(5, 1);
    lcd_puts("World!");

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