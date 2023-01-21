#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "lcdsim.h"

LCDSim* LCDSim_Create(SDL_Surface *screen, int x, int y) {

    LCDSim* self = malloc(sizeof(LCDSim));
    if (self != NULL) {
        // HD44780_Init(&self->mcu);
        // GraphicUnit_Init(&self->gu);
        self->gu.screen = screen;
        self->gu.on_screen.x = x;
        self->gu.on_screen.y = y;
        self->lastTime = SDL_GetTicks();
    }
    return self;
}