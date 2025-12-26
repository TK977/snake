#ifndef SETTINGS_UI_H
#define SETTINGS_UI_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

void ShowSettingsUI(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* font, TTF_Font* smallFont);

#endif // SETTINGS_UI_H
