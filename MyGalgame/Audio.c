#define _CRT_SECURE_NO_WARNINGS
#include "Audio.h"
#include <string.h>

#define CHUNK 4096                 // 每次向 SDL 音频流投喂的数据字节数（4 KB）

/* ---------- 全局静态变量 ---------- */
static SDL_AudioStream* stream = NULL;   // SDL3 音频流对象，用于缓冲音频数据
static SDL_AudioDeviceID device = 0;     // 打开的音频播放设备 ID
static SDL_Thread* feedThread = NULL;    // 后台线程，负责持续向音频流喂数据
static Uint8* wavBuf = NULL;             // 整个 WAV 文件读入后的内存缓冲区
static Uint32 wavLen = 0;                // 缓冲区总字节数
static Uint32 playPos = 0;               // 当前播放位置（字节偏移）
static bool isLooping = false;           // 是否循环播放
static bool g_alive = false;             // 全局“音频子系统存活”标志，控制线程退出

/* 后台音频投喂线程函数
 * 不断检查音频流剩余数据，低于阈值就拷贝新数据进去，实现无缝播放 */
static int SDLCALL AudioFeedThread(void* userdata)
{
    while (g_alive)                       // 主循环，系统关闭时置 false
    {
        if (stream)
        {
            /* 只要队列里少于 16 个 CHUNK（64 KB）就再喂 4 KB */
            Sint32 queued = SDL_GetAudioStreamQueued(stream);
            if (queued < 16 * CHUNK)
            {
                Uint8  tmp[CHUNK];        // 临时缓冲区
                Uint32 left = wavLen - playPos;   // 距离文件末尾还剩多少字节
                Uint32 tocpy = (left > CHUNK ? CHUNK : left); // 本次能拷贝多少
                memcpy(tmp, wavBuf + playPos, tocpy);
                playPos += tocpy;

                if (tocpy < CHUNK && isLooping)          // 需要循环：把开头数据补进来
                {
                    Uint32 need = CHUNK - tocpy;
                    Uint32 cp = (need > wavLen ? wavLen : need);
                    memcpy(tmp + tocpy, wavBuf, cp);
                    playPos = cp;       // 播放位置回到头部偏移 cp 处
                }
                else if (tocpy < CHUNK)                  // 不循环：剩余补 0（静音）
                {
                    memset(tmp + tocpy, 0, CHUNK - tocpy);
                }
                SDL_PutAudioStreamData(stream, tmp, CHUNK); // 送进音频流
                SDL_Log("feed %d bytes, pos=%u", CHUNK, playPos);
            }
        }
        SDL_Delay(10);  // 每 10 ms 检查一次，避免 CPU 占用过高
    }
    return 0;
}

/* 初始化音频子系统：加载 WAV → 打开音频设备 → 创建音频流 → 启动后台线程
 * file：相对路径 WAV 文件名（建议放在 exe 同级目录）
 * 返回值：true=成功，false=失败 */
bool Audio_Init(const char* file)
{
    /* 1. 如果已经初始化，直接返回，防止重复打开 */
    if (g_alive)
    {
        SDL_Log("Audio_Init blocked: already initialized");
        return true;
    }

    /* 2. 加载 WAV 文件到内存 */
    SDL_AudioSpec spec;
    if (!SDL_LoadWAV(file, &spec, &wavBuf, &wavLen))
    {
        SDL_Log("LoadWAV failed: %s", SDL_GetError());
        return false;
    }

    /* 3. 打开音频播放设备（使用 WAV 自身的格式）*/
    device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    if (device == 0)
    {
        SDL_Log("OpenAudioDevice failed: %s", SDL_GetError());
        SDL_free(wavBuf);
        wavBuf = NULL;
        return false;
    }

    /* 4. 创建音频流（输入输出格式相同，直接透传）*/
    stream = SDL_CreateAudioStream(&spec, &spec);
    if (!stream)
    {
        SDL_Log("CreateAudioStream failed: %s", SDL_GetError());
        SDL_CloseAudioDevice(device);
        SDL_free(wavBuf);
        wavBuf = NULL;
        return false;
    }
    SDL_BindAudioStream(device, stream);   // 把流绑定到设备

    /* 5. 启动后台投喂线程 */
    g_alive = true;                        // 先置标志，再创建线程
    feedThread = SDL_CreateThread(AudioFeedThread, "AudioFeed", NULL);

    SDL_Log("Audio device opened OK, id=%u", device);
    return true;
}

/* 开始/重新开始播放
 * loop：true=循环播放，false=播放一次 */
void Audio_Play(bool loop)
{
    if (!g_alive || !stream) return;   // 未初始化直接返回
    isLooping = loop;                  // 记录循环标志
    playPos = 0;                       // 播放位置归零
    SDL_ResumeAudioDevice(device);     // 解除设备暂停，真正开始出声
}

/* 关闭音频子系统：顺序停止线程 → 销毁流 → 关闭设备 → 释放 WAV 内存
 * 可安全重复调用 */
void Audio_Quit(void)
{
    if (!g_alive) return;   // 防止重复释放
    g_alive = false;        // 先让线程自然退出

    if (feedThread)
    {
        SDL_WaitThread(feedThread, NULL); // 等待线程结束
        feedThread = NULL;
    }
    if (stream)
    {
        SDL_DestroyAudioStream(stream);
        stream = NULL;
    }
    if (device)
    {
        SDL_CloseAudioDevice(device);
        device = 0;
    }
    if (wavBuf)
    {
        SDL_free(wavBuf);
        wavBuf = NULL;
    }
}