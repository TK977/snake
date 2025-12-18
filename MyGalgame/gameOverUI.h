#ifndef GAMEOVERUI_H
#define GAMEOVERUI_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include "StartUI.h" // 使用 Button 结构体

// 游戏结束原因
typedef enum {
    GAME_OVER_WALL_COLLISION,    // 撞墙
    GAME_OVER_SELF_COLLISION,    // 撞到自己
    GAME_OVER_QUIT              // 主动退出
} GameOverReason;

// 游戏结束数据
typedef struct {
    int finalScore;
    int highScore;
    int level;
    int length;
    int timeElapsed;
    GameOverReason reason;
    bool isNewHighScore;
} GameOverData;

// 游戏结束界面返回值
typedef enum {
    GAMEOVER_RESTART,    // 重新开始
    GAMEOVER_MENU,       // 返回主菜单
    GAMEOVER_EXIT        // 退出游戏
} GameOverAction;

// 主函数
GameOverAction showGameOverUI(SDL_Renderer* renderer,
    TTF_Font* titleFont,
    TTF_Font* normalFont,
    TTF_Font* smallFont,
    GameOverData* data);

// 辅助函数
void renderGameOverBackground(SDL_Renderer* renderer);
void drawTitle(SDL_Renderer* renderer, TTF_Font* font, char* text);
void renderGameOverStats(SDL_Renderer* renderer, TTF_Font* font, GameOverData* data);
void renderNewHighScoreAnima(SDL_Renderer* renderer, TTF_Font* font,
    int centerX, int y, Uint32 time);
//const char* getGameOverMessage(GameOverReason reason);

#endif // GAMEOVERUI_H