#pragma once
#ifndef MULTIGAMEOVERUI_H
#define MULTIGAMEOVERUI_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

// Enums
typedef enum {
    MP_GAME_OVER_DRAW,
    MP_GAME_OVER_PLAYER1_WIN,
    MP_GAME_OVER_PLAYER2_WIN
} MultiPlayerGameOverReason;

typedef enum {
    MP_GAMEOVER_REMATCH,
    MP_GAMEOVER_MENU,
    MP_GAMEOVER_EXIT
} MultiPlayerGameOverAction;

// Structs
typedef struct {
    char name[32];
    SDL_Color color;
    int score;
    int length;
    int kills;
    bool isAlive;
    bool isWinner;
} PlayerGameOverInfo;

typedef struct {
    MultiPlayerGameOverReason reason;
    int totalTime;
    int level;
    PlayerGameOverInfo player1;
    PlayerGameOverInfo player2;
} MultiPlayerGameOverData;

// Function Prototypes
MultiPlayerGameOverAction showMultiPlayerGameOverUI(SDL_Renderer* renderer,
    TTF_Font* titleFont,
    TTF_Font* buttonFont,
    TTF_Font* smallFont,
    MultiPlayerGameOverData* data);

#endif // MULTIGAMEOVERUI_H