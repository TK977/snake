/**
 * GamePlay.c
 * 单/双人贪吃蛇核心玩法实现
 * 包括：食物系统、加速食物、碰撞判定、双人对战、时间限制、结算界面
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "GamePlay.h"
#include "Snake.h"
#include "StartUI.h"
#include "gameOverUI.h"
#include <stdlib.h>
#include <time.h>
#include "MultiGameOverUI.h"
#include "Data.h"
#include "Settings.h"

 /**
  * 纯软件画“圆角矩形”——SDL3 暂无 API，用 4 个实心圆 + 十字拼凑
  * @param r 渲染器
  * @param rect 目标矩形
  * @param c 颜色
  */
static void RenderRoundRect(SDL_Renderer* r, const SDL_FRect* rect, SDL_Color c)
{
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);

    /* 1. 十字主体 */
    SDL_FRect core;
    core = (SDL_FRect){ rect->x + 6, rect->y, rect->w - 12, rect->h };
    SDL_RenderFillRect(r, &core);
    core = (SDL_FRect){ rect->x, rect->y + 6, rect->w, rect->h - 12 };
    SDL_RenderFillRect(r, &core);

    /* 2. 四个角画实心圆（半径 6） */
    float rad = 6.f;
    int   steps = 12;
    for (int i = 0; i < 4; ++i) {
        float cx = rect->x + (i & 1 ? rect->w - rad : rad);
        float cy = rect->y + (i >> 1 ? rect->h - rad : rad);
        for (int j = 0; j < steps; ++j) {
            float a = (float)j / steps * 2.f * 3.1415926f;
            SDL_FRect p = { cx + cosf(a) * rad, cy + sinf(a) * rad, 1.f, 1.f };
            SDL_RenderFillRect(r, &p);
        }
    }
}

/**
 * 检测蛇头是否碰到另一条蛇的身体（双人用）
 */
static bool HitOtherSnake(const Snake* me, const Snake* other)
{
    SDL_Point head = Snake_HeadGrid(me);
    for (Node* cur = other->head; cur; cur = cur->next)
        if (cur->x == head.x && cur->y == head.y) return true;
    return false;
}

/* 食物类型枚举及颜色 */
typedef enum { FOOD_NORMAL, FOOD_BONUS, FOOD_SPEED, FOOD_NUM } FoodType;
typedef struct { int x, y; FoodType type; } Food;

static SDL_Color foodColor(FoodType t) {
    switch (t) {
    case FOOD_NORMAL: return (SDL_Color) { 255, 255, 255, 255 };
    case FOOD_BONUS:  return (SDL_Color) { 255, 215, 0, 255 };
    case FOOD_SPEED:  return (SDL_Color) { 0, 255, 255, 255 };
    }
    return (SDL_Color) { 255, 255, 255, 255 };
}

/**
 * 判断 (x,y) 是否被蛇身占用
 */
static bool occupied(const Snake* s, int x, int y) {
    for (Node* cur = s->head; cur; cur = cur->next)
        if (cur->x == x && cur->y == y) return true;
    return false;
}

/**
 * 生成新食物，位置不与蛇冲突
 */
static Food newFood(const Snake* s) {
    Food f;
    do { f.x = rand() % GRID_W; f.y = rand() % GRID_H; } while (occupied(s, f.x, f.y));
    f.type = (FoodType)(rand() % FOOD_NUM);
    return f;
}

/**
 * 单局渲染函数：背景、边框、蛇、食物、分数
 * @param id 1=单人/玩家1  2=玩家2（决定颜色）
 */
static void drawGame(SDL_Renderer* renderer, const Snake* s, const Food* f,
    TTF_Font* font, int score, int id)
{
    SDL_SetRenderDrawColor(renderer, 20, 30, 40, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 100, 150, 200, 255);
    SDL_FRect border = { 0,0, GRID_W * CELL, GRID_H * CELL };
    SDL_RenderRect(renderer, &border);

    /* 蛇头圆角 */
    Node* cur = s->head;
    SDL_FRect headR = { cur->x * CELL, cur->y * CELL, CELL, CELL };
    SDL_Color headColor = id == 1
        ? currentSettings.snakeColor
        : (SDL_Color) { 100, 255, 255, 255 };
    RenderRoundRect(renderer, &headR, headColor);

    /* 蛇身方块 */
    for (cur = cur->next; cur; cur = cur->next) {
        SDL_FRect r = { cur->x * CELL, cur->y * CELL, CELL, CELL };
        SDL_SetRenderDrawColor(renderer,
            id == 1 ? currentSettings.snakeColor.r : 100,
            id == 1 ? currentSettings.snakeColor.g : 255,
            id == 1 ? currentSettings.snakeColor.b : 255,
            255);
        SDL_RenderFillRect(renderer, &r);
    }

    /* 食物 */
    SDL_FRect rf = { f->x * CELL, f->y * CELL, CELL, CELL };
    SDL_Color c = foodColor(f->type);
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(renderer, &rf);

    /* 分数文字 */
    char buf[64];
    snprintf(buf, sizeof(buf), "P%d Score: %d", id, score);
    SDL_Surface* surf = TTF_RenderText_Blended(font, buf, 0, (SDL_Color) { 255, 255, 255, 255 });
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FRect dst = { 10, 10, surf->w, surf->h };
    SDL_RenderTexture(renderer, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
    SDL_DestroySurface(surf);

    SDL_RenderPresent(renderer);
}

/**
 * 单人模式主循环
 */
void StartSinglePlayer(SDL_Renderer* renderer, TTF_Font* smallFont)
{
    srand((unsigned)time(NULL));
    Snake* s = Snake_Create(GRID_W / 2, GRID_H / 2);
    Food f = newFood(s);
    int score = 0, high = 0;
    Uint32 last = SDL_GetTicks(), spd = 120; // 基础速度 120 ms
    bool boost = false;   // 是否处于加速状态
    bool run = true;
    Uint32 boostEnd = 0;  // 加速结束时刻

    while (run) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) { run = false; break; }
            if (e.type == SDL_EVENT_KEY_DOWN) {
                switch (e.key.scancode) {
                case SDL_SCANCODE_UP:    if (s->dir != DIR_DOWN)  s->dir = DIR_UP;    break;
                case SDL_SCANCODE_DOWN:  if (s->dir != DIR_UP)    s->dir = DIR_DOWN;  break;
                case SDL_SCANCODE_LEFT:  if (s->dir != DIR_RIGHT) s->dir = DIR_LEFT;  break;
                case SDL_SCANCODE_RIGHT: if (s->dir != DIR_LEFT)  s->dir = DIR_RIGHT; break;
                }
            }
        }

        Uint32 now = SDL_GetTicks();
        if (now - last < (boost ? 60 : spd)) { SDL_Delay(1); continue; } // 加速时 60 ms
        last = now;

        Snake_Move(s);
        if (Snake_WallCollision(s) || Snake_SelfCollision(s)) break; // 撞墙/自撞

        SDL_Point h = Snake_HeadGrid(s);
        if (h.x == f.x && h.y == f.y) {              // 吃到食物
            score += (f.type == FOOD_BONUS ? 3 : 1); // 奖励食物 +3 分
            if (f.type == FOOD_SPEED) { boost = true; boostEnd = now + 5000; } // 5 秒加速
            Snake_Grow(s);
            f = newFood(s);
        }
        if (boost && now > boostEnd) boost = false;  // 加速结束

        drawGame(renderer, s, &f, smallFont, score, 1);
    }

    /* 封装结算数据 */
    GameOverData data = { 0 };
    data.finalScore = score;
    data.highScore = high;                 // 这里未读存档，high=0
    data.level = 1;
    data.length = 3;
    data.timeElapsed = SDL_GetTicks() / 1000;
    data.isNewHighScore = score > high;
    data.reason = GAME_OVER_SELF_COLLISION;

    SaveRanking(currentSettings.nickname, score); // 存档
    showGameOverUI(renderer, NULL, smallFont, smallFont, &data);
    Snake_Destroy(s);
}

/**
 * 双人模式主循环
 * @return 玩家选择动作（重开/菜单/退出）
 */
GameOverAction StartMultiPlayer(SDL_Renderer* renderer,
    TTF_Font* titleFont,
    TTF_Font* buttonFont,
    TTF_Font* smallFont)
{
    srand((unsigned)time(NULL));
    /* 创建两条蛇，左右对称出生 */
    Snake* s1 = Snake_Create(10, GRID_H / 2 - 10);
    Snake* s2 = Snake_Create(GRID_W - 10, GRID_H / 2 + 10);
    /* 把 s2 初始三节点向右摆 */
    s2->head->next->x = GRID_W - 9;
    s2->head->next->next->x = GRID_W - 8;
    s1->dir = DIR_RIGHT;
    s2->dir = DIR_LEFT;

    Food f = newFood(s1);
    int sc1 = 0, sc2 = 0;
    Uint32 last = SDL_GetTicks(), spd = 120;
    bool run = true;
    Uint32 gameStart = SDL_GetTicks();
    const Uint32 TIME_LIMIT_MS = 2 * 60 * 1000; // 2 分钟时限

    bool d1 = false, d2 = false; // 死亡标志

    /* 双人结算数据结构（先占位，后填真实数据） */
    MultiPlayerGameOverData mpData = { 0 };
    mpData.reason = MP_GAME_OVER_DRAW;
    mpData.totalTime = 0;
    mpData.level = 1;
    strcpy(mpData.player1.name, "P1");
    strcpy(mpData.player2.name, "P2");
    mpData.player1.color = (SDL_Color){ 0, 255, 100, 255 };
    mpData.player2.color = (SDL_Color){ 100, 150, 255, 255 };
    mpData.player1.score = sc1;
    mpData.player2.score = sc2;
    mpData.player1.length = 3;
    mpData.player2.length = 3;
    mpData.player1.kills = 0;
    mpData.player2.kills = 0;
    mpData.player1.isAlive = true;
    mpData.player2.isAlive = true;
    mpData.player1.isWinner = false;
    mpData.player2.isWinner = false;

    while (run) {
        Uint32 now = SDL_GetTicks();
        Uint32 elapsed = now - gameStart;
        if (elapsed >= TIME_LIMIT_MS) { run = false; break; } // 时间到

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) { run = false; break; }
            /* P1：WASD */
            if (e.type == SDL_EVENT_KEY_DOWN) {
                switch (e.key.scancode) {
                case SDL_SCANCODE_W: if (s1->dir != DIR_DOWN)  s1->dir = DIR_UP;    break;
                case SDL_SCANCODE_S: if (s1->dir != DIR_UP)    s1->dir = DIR_DOWN;  break;
                case SDL_SCANCODE_A: if (s1->dir != DIR_RIGHT) s1->dir = DIR_LEFT;  break;
                case SDL_SCANCODE_D: if (s1->dir != DIR_LEFT)  s1->dir = DIR_RIGHT; break;
                    /* P2：方向键 */
                case SDL_SCANCODE_UP:    if (s2->dir != DIR_DOWN)  s2->dir = DIR_UP;    break;
                case SDL_SCANCODE_DOWN:  if (s2->dir != DIR_UP)    s2->dir = DIR_DOWN;  break;
                case SDL_SCANCODE_LEFT:  if (s2->dir != DIR_RIGHT) s2->dir = DIR_LEFT;  break;
                case SDL_SCANCODE_RIGHT: if (s2->dir != DIR_LEFT)  s2->dir = DIR_RIGHT; break;
                }
            }
        }

        if (now - last < spd) { SDL_Delay(1); continue; }
        last = now;

        Snake_Move(s1); Snake_Move(s2);

        /* 各种死亡判定 */
        bool wall1 = Snake_WallCollision(s1), self1 = Snake_SelfCollision(s1), hit1 = HitOtherSnake(s1, s2);
        bool wall2 = Snake_WallCollision(s2), self2 = Snake_SelfCollision(s2), hit2 = HitOtherSnake(s2, s1);
        d1 = wall1 || self1 || hit1;
        d2 = wall2 || self2 || hit2;

        if (d1 || d2) break; // 有人死亡就结束

        /* 吃食物 */
        SDL_Point h1 = Snake_HeadGrid(s1), h2 = Snake_HeadGrid(s2);
        bool eat = false;
        if (h1.x == f.x && h1.y == f.y) { sc1 += (f.type == FOOD_BONUS ? 3 : 1); Snake_Grow(s1); eat = true; }
        if (h2.x == f.x && h2.y == f.y) { sc2 += (f.type == FOOD_BONUS ? 3 : 1); Snake_Grow(s2); eat = true; }
        if (eat) f = newFood(s1);

        /* 双蛇渲染 */
        SDL_SetRenderDrawColor(renderer, 20, 30, 40, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 100, 150, 200, 255);
        SDL_FRect border = { 0,0, GRID_W * CELL, GRID_H * CELL };
        SDL_RenderRect(renderer, &border);

        /* P1 蛇头圆角 */
        Node* cur = s1->head;
        SDL_FRect headR1 = { cur->x * CELL, cur->y * CELL, CELL, CELL };
        RenderRoundRect(renderer, &headR1, (SDL_Color) { 0, 255, 100, 255 });
        /* P1 身子 */
        for (cur = cur->next; cur; cur = cur->next) {
            SDL_FRect r = { cur->x * CELL, cur->y * CELL, CELL, CELL };
            SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
            SDL_RenderFillRect(renderer, &r);
        }
        /* P2 蛇头圆角 */
        cur = s2->head;
        SDL_FRect headR2 = { cur->x * CELL, cur->y * CELL, CELL, CELL };
        RenderRoundRect(renderer, &headR2, (SDL_Color) { 100, 150, 255, 255 });
        /* P2 身子 */
        for (cur = cur->next; cur; cur = cur->next) {
            SDL_FRect r = { cur->x * CELL, cur->y * CELL, CELL, CELL };
            SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255);
            SDL_RenderFillRect(renderer, &r);
        }
        /* 食物 */
        SDL_FRect rf = { f.x * CELL, f.y * CELL, CELL, CELL };
        SDL_Color c = foodColor(f.type);
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_RenderFillRect(renderer, &rf);

        /* 分数文字 */
        char buf[64];
        snprintf(buf, sizeof(buf), "P1:%d  P2:%d", sc1, sc2);
        SDL_Surface* surf = TTF_RenderText_Blended(smallFont, buf, 0, (SDL_Color) { 255, 255, 255, 255 });
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FRect dst = { 10, 10, surf->w, surf->h };
        SDL_RenderTexture(renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
        SDL_DestroySurface(surf);

        SDL_RenderPresent(renderer);
    }

    /* 结算数据 */
    GameOverData data = { 0 };
    data.finalScore = sc1 + sc2;
    data.highScore = 0;
    data.level = 2;
    data.length = 3;
    data.timeElapsed = SDL_GetTicks() / 1000;
    data.isNewHighScore = false;
    data.reason = GAME_OVER_SELF_COLLISION;

    /* 统一胜负 & 存活 & 时间 */
    mpData.totalTime = (SDL_GetTicks() - gameStart) / 1000;
    mpData.player1.isAlive = !d1;
    mpData.player2.isAlive = !d2;
    mpData.player1.score = sc1;
    mpData.player2.score = sc2;

    if (d1 && !d2) {                 // P1 死 P2 活
        mpData.reason = MP_GAME_OVER_PLAYER2_WIN;
        mpData.player2.isWinner = true;
    }
    else if (!d1 && d2) {            // P2 死 P1 活
        mpData.reason = MP_GAME_OVER_PLAYER1_WIN;
        mpData.player1.isWinner = true;
    }
    else if (sc1 > sc2) {            // 都活或都死，比分数
        mpData.reason = MP_GAME_OVER_PLAYER1_WIN;
        mpData.player1.isWinner = true;
    }
    else if (sc2 > sc1) {
        mpData.reason = MP_GAME_OVER_PLAYER2_WIN;
        mpData.player2.isWinner = true;
    }
    else {                           // 分数相同
        mpData.reason = MP_GAME_OVER_DRAW;
    }

    /* 弹出双人结算界面 */
    MultiPlayerGameOverAction mpAct =
        showMultiPlayerGameOverUI(renderer, titleFont, buttonFont, smallFont, &mpData);

    /* 把新枚举映射回旧枚举，方便外层循环 */
    GameOverAction act;
    switch (mpAct) {
    case MP_GAMEOVER_REMATCH: act = GAMEOVER_RESTART; break;
    case MP_GAMEOVER_MENU:    act = GAMEOVER_MENU;    break;
    case MP_GAMEOVER_EXIT:    act = GAMEOVER_EXIT;    break;
    default:                  act = GAMEOVER_MENU;    break;
    }
    Snake_Destroy(s1);
    Snake_Destroy(s2);
    return act;
}