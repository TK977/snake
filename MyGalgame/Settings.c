#include "Settings.h"
#include <string.h>

GameSettings currentSettings;

void InitSettings(void) {
    // 默认设置
    strcpy_s(currentSettings.nickname, sizeof(currentSettings.nickname), "Player");
    currentSettings.snakeColor = (SDL_Color){ 0, 255, 100, 255 }; // Default Green
}
//设置昵称
void SetNickname(const char* name) {
    if (name && strlen(name) > 0) {
        strcpy_s(currentSettings.nickname, sizeof(currentSettings.nickname), name);
    }
}
//设置蛇的颜色
void SetSnakeColor(SDL_Color color) {
    currentSettings.snakeColor = color;
}
