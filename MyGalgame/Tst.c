#include <SDL3/SDL.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif

int main1(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init failed: '%s'\\n", SDL_GetError());
#ifdef _WIN32
        DWORD e = GetLastError();
        if (e) {
            char buf[512] = { 0 };
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, e, 0, buf, sizeof(buf), NULL);
            printf("Win32 GetLastError: %u: %s\\n", (unsigned)e, buf);
        }
#endif
        return 1;
    }
    printf("SDL_Init OK\\n");
    SDL_Quit();
    return 0;
}