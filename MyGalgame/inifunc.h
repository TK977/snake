#pragma once
#ifndef INIFUNC_H
#define INIFUNC_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

// 初始化 SDL 子系统（音频、视频、TTF 等）
// 返回 true 表示成功，false 表示失败
bool initSDLsubsystem(void);

// 反初始化 SDL 子系统（释放资源）
void uninitSDLsubsystem(void);

#endif // INIFUNC_H