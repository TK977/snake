#pragma once
#include <SDL3/SDL.h>

bool Audio_Init(const char* wavFileRelativePath);   // 加载并打开音频设备
void Audio_Play(bool loop);                         // 开始播放（loop=true 循环）
void Audio_Quit(void);                              // 关闭设备并释放内存
