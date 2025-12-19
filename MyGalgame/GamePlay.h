#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

/* 供 StartUI.c 的 case 0/1 直接调用 */
void StartSinglePlayer(SDL_Renderer* renderer, TTF_Font* smallFont);
void StartMultiPlayer(SDL_Renderer* renderer, TTF_Font* smallFont);

#endif