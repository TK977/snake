#define _CRT_SECURE_NO_WARNINGS
#include "Audio.h"
#include <string.h>

#define CHUNK 4096

/* ---------- 全局单例对象 ---------- */
static SDL_AudioStream* stream = NULL;
static SDL_AudioDeviceID device = 0;
static SDL_Thread* feedThread = NULL;
static Uint8* wavBuf = NULL;
static Uint32           wavLen = 0;
static Uint32           playPos = 0;
static bool             isLooping = false;
static bool             g_alive = false;   // 全局生命周期标志

/* 后台喂数据线程 */
static int SDLCALL AudioFeedThread(void* userdata)
{
    while (g_alive)
    {
        if (stream)
        {
            /* 只要队列低于 64 KB 就再塞 4 KB */
            Sint32 queued = SDL_GetAudioStreamQueued(stream);
            if (queued < 16 * CHUNK)
            {
                Uint8  tmp[CHUNK];
                Uint32 left = wavLen - playPos;
                Uint32 tocpy = (left > CHUNK ? CHUNK : left);
                memcpy(tmp, wavBuf + playPos, tocpy);
                playPos += tocpy;

                if (tocpy < CHUNK && isLooping)          // 循环
                {
                    Uint32 need = CHUNK - tocpy;
                    Uint32 cp = (need > wavLen ? wavLen : need);
                    memcpy(tmp + tocpy, wavBuf, cp);
                    playPos = cp;
                }
                else if (tocpy < CHUNK)                  // 不循环补 0
                {
                    memset(tmp + tocpy, 0, CHUNK - tocpy);
                }
                SDL_PutAudioStreamData(stream, tmp, CHUNK);
                SDL_Log("feed %d bytes, pos=%u", CHUNK, playPos);
            }
        }
        SDL_Delay(10);
    }
    return 0;
}

bool Audio_Init(const char* file)
{
    /* 1. 全局单例：已初始化则直接返回 */
    if (g_alive)
    {
        SDL_Log("Audio_Init blocked: already initialized");
        return true;
    }

    /* 2. 加载 WAV */
    SDL_AudioSpec spec;
    if (!SDL_LoadWAV(file, &spec, &wavBuf, &wavLen))
    {
        SDL_Log("LoadWAV failed: %s", SDL_GetError());
        return false;
    }

    /* 3. 打开播放设备 */
    device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    if (device == 0)
    {
        SDL_Log("OpenAudioDevice failed: %s", SDL_GetError());
        SDL_free(wavBuf);
        wavBuf = NULL;
        return false;
    }

    /* 4. 创建音频流并绑定 */
    stream = SDL_CreateAudioStream(&spec, &spec);
    if (!stream)
    {
        SDL_Log("CreateAudioStream failed: %s", SDL_GetError());
        SDL_CloseAudioDevice(device);
        SDL_free(wavBuf);
        wavBuf = NULL;
        return false;
    }
    SDL_BindAudioStream(device, stream);

    /* 5. 启动后台喂数据线程 */
    g_alive = true;   // 必须先置标志，再创建线程
    feedThread = SDL_CreateThread(AudioFeedThread, "AudioFeed", NULL);

    SDL_Log("Audio device opened OK, id=%u", device);
    return true;
}

void Audio_Play(bool loop)
{
    if (!g_alive || !stream) return;
    isLooping = loop;
    playPos = 0;          // 从头开始
    SDL_ResumeAudioDevice(device);
}

void Audio_Quit(void)
{
    if (!g_alive) return;   // 重复释放防护
    g_alive = false;        // 让线程自然退出

    if (feedThread)
    {
        SDL_WaitThread(feedThread, NULL);
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