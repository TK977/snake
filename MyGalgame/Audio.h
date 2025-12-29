#pragma once
#include <SDL3/SDL.h>

/* 初始化音频：加载指定 WAV 文件并打开播放设备
 * wavFileRelativePath：相对路径 WAV 文件名
 * 返回 true 表示成功，false 表示失败 */
bool Audio_Init(const char* wavFileRelativePath);

/* 开始播放。调用前必须先 Audio_Init 成功
 * loop == true  循环播放
 * loop == false 只播放一次（到结尾自动静音）*/
void Audio_Play(bool loop);

/* 关闭音频设备并释放所有内存。可重复调用，内部已做保护 */
void Audio_Quit(void);