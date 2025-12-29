/**
 * inifunc.h
 * SDL 子系统初始化和释放封装
 */

#pragma once
#ifndef INIFUNC_H
#define INIFUNC_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

 /**
  * 初始化 SDL 视频、音频、TTF、BGM
  * @return true 成功
  */
bool initSDLsubsystem(void);

/**
 * 释放所有 SDL 资源
 */
void uninitSDLsubsystem(void);

#endif // INIFUNC_H