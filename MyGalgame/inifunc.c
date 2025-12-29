/**
 * inifunc.c
 * 一次性初始化/释放 SDL 视频、音频、TTF、音频播放
 */

#include "inifunc.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include "Audio.h"

 /**
  * 初始化 SDL 视频、音频、TTF，并循环播放 BGM
  * @return true 成功
  */
bool initSDLsubsystem(void)
{
    /* 初始化视频 + 音频 */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == false) {
        printf("SDL_Init Error: %s", SDL_GetError());
        return false;
    }
    /* 初始化 TTF */
    if (TTF_Init() == false) {
        SDL_Log("TTF_Init Error: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }
    /* 加载并循环播放背景音乐（文件放在 exe 同级目录） */
    if (!Audio_Init("music.wav")) {
        SDL_Log("BGM load failed, running without audio");
    }
    else {
        Audio_Play(true); // 循环播放
    }
    return true;
}

/**
 * 逆序释放：音频 → TTF → SDL
 */
void uninitSDLsubsystem(void)
{
    Audio_Quit();
    TTF_Quit();
    SDL_Quit();
}