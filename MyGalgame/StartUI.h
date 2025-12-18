#pragma once
#ifndef STARTUI_H
#define STARTUI_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include "inifunc.h"
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080


typedef struct {
    SDL_FRect rect;
    const char* text;
    SDL_Color textColor;
    SDL_Color normalColor;
    SDL_Color hoverColor;
    SDL_Color pressedColor;
    bool isHovered;
    bool isPressed;
    bool isEnabled;
} Button;

extern SDL_Window* win;
extern SDL_Renderer* renderer;
extern TTF_Font* titleFont;
extern TTF_Font* buttonFont;
extern TTF_Font* smallFont;

// 初始化和清理
bool initSDLsubsystem(void);
void uninitSDLsubsystem(void);

// 按钮相关
Button create_button(float x, float y, float w, float h, const char* text);
bool isPointInButton(Button* btn, float x, float y);
void renderButton(SDL_Renderer* renderer, Button* btn, TTF_Font* font);


// 主界面
void StartUICreate(void);

#endif // STARTUI_H