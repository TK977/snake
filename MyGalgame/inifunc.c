#include "inifunc.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

bool initSDLsubsystem(void) {
    if (SDL_Init(SDL_INIT_VIDEO| SDL_INIT_AUDIO) == false) { 
        printf("SDL_Init Error: %s", SDL_GetError());
        return false;
    }
    if (TTF_Init() == false) {
        SDL_Log("TTF_Init Error: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }
    return true;
}

void uninitSDLsubsystem(void) {
    TTF_Quit();
    SDL_Quit();
}
