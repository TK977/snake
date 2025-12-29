/**
 * StartUI.h
 * 主菜单对外接口及全局声明
 */

#pragma once
#ifndef STARTUI_H
#define STARTUI_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include "inifunc.h"
#include "gameOverUI.h"

 /* 窗口常量 */
#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080

/* 按钮结构体 */
typedef struct {
    SDL_FRect rect;                 // 矩形区域
    const char* text;               // 文字
    SDL_Color textColor;            // 文字颜色
    SDL_Color normalColor;          // 正常背景
    SDL_Color hoverColor;           // 悬停背景
    SDL_Color pressedColor;         // 按下背景
    bool isHovered;
    bool isPressed;
    bool isEnabled;                 // 是否可用
} Button;

/* 全局窗口句柄，供设置界面使用 */
extern SDL_Window* win;

/* 函数声明 */
bool initSDLsubsystem(void);
void uninitSDLsubsystem(void);

Button create_button(float x, float y, float w, float h, const char* text);
bool isPointInButton(Button* btn, float x, float y);
void renderButton(SDL_Renderer* renderer, Button* btn, TTF_Font* font);

/* 主菜单入口 */
void StartUICreate(void);

#endif // STARTUI_H