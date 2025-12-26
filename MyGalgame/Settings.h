#ifndef SETTINGS_H
#define SETTINGS_H

#include <SDL3/SDL.h>

typedef struct {
    char nickname[32];
    SDL_Color snakeColor;
} GameSettings;

extern GameSettings currentSettings;

void InitSettings(void);
void SetNickname(const char* name);
void SetSnakeColor(SDL_Color color);

#endif // SETTINGS_H
