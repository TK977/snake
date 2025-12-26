#include "inifunc.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include "Audio.h"

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
    // ---------- 加载并播放 BGM ----------
    if (!Audio_Init("music.wav")) {          // 文件已拷到 exe 旁边
        SDL_Log("BGM load failed, running without audio");
    }
    else {
        Audio_Play(true);                         // 循环播放
    }
    return true;
}

void uninitSDLsubsystem(void) {
    Audio_Quit();
    TTF_Quit();
    SDL_Quit();
	
}
