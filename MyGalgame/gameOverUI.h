#ifndef GAMEOVERUI_H
#define GAMEOVERUI_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include "StartUI.h" // 使用 Button 结构体

/* 游戏结束原因枚举，方便以后扩展不同提示语 */
typedef enum {
    GAME_OVER_WALL_COLLISION,    // 撞墙
    GAME_OVER_SELF_COLLISION,    // 撞到自己
    GAME_OVER_QUIT               // 主动退出
} GameOverReason;

/* 游戏结束界面需要显示的数据包 */
typedef struct {
    int finalScore;     // 本局得分
    int highScore;      // 历史最高分（用于判断新纪录）
    int level;          // 关卡（暂留）
    int length;         // 蛇长度（暂留）
    int timeElapsed;    // 游戏耗时（秒）
    GameOverReason reason; // 死亡原因（暂留）
    bool isNewHighScore;   // 标志：是否打破最高分
} GameOverData;

/* showGameOverUI 函数的返回值，告诉上层界面下一步去哪 */
typedef enum {
    GAMEOVER_RESTART,    // 重新开始
    GAMEOVER_MENU,       // 返回主菜单
    GAMEOVER_EXIT        // 退出游戏
} GameOverAction;

/* ========== 主函数：进入阻塞式事件循环，直到用户选择某项 ========== */
GameOverAction showGameOverUI(SDL_Renderer* renderer,
    TTF_Font* titleFont,
    TTF_Font* normalFont,
    TTF_Font* smallFont,
    GameOverData* data);

/* ========== 辅助函数：供内部或外部调用 ========== */
// 渲染背景、面板、边框
void renderGameOverBackground(SDL_Renderer* renderer);

// 渲染大标题（“Game Over”或“Victory!”）
void drawTitle(SDL_Renderer* renderer, TTF_Font* font, char* text);

// 渲染分数、等级、长度、时间等详细数据
void renderGameOverStats(SDL_Renderer* renderer, TTF_Font* font, GameOverData* data);

// 渲染“NEW HIGH SCORE!”动画（缩放+闪烁）
void renderNewHighScoreAnima(SDL_Renderer* renderer, TTF_Font* font,
    int centerX, int y, Uint32 time);

#endif // GAMEOVERUI_H