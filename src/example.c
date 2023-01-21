#include "lcdsim.h"

int main (int argc, char** argv) {
    SDL_Surface *screen = NULL;
    LCDSim *lcd = NULL;

    /* Initialize the LCD object */
    lcd = LCDSim_Create(screen, 0, 0);
}