#include "GamePlay.h"
#include "Snake.h"
#include "StartUI.h"
#include "gameOverUI.h"
#include <stdlib.h>
#include <time.h>
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>   // snprintf

/* 检测蛇头是否碰到另一条蛇的身体 */
static bool HitOtherSnake(const Snake* me, const Snake* other)
{
    SDL_Point head = Snake_HeadGrid(me);
    for (Node* cur = other->head; cur; cur = cur->next)
        if (cur->x == head.x && cur->y == head.y) return true;
    return false;
}

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

static bool occupied(const Snake* s, int x, int y) {
    for (Node* cur = s->head; cur; cur = cur->next)
        if (cur->x == x && cur->y == y) return true;
    return false;
}

static Food newFood(const Snake* s) {
    Food f;
    do { f.x = rand() % GRID_W; f.y = rand() % GRID_H; } while (occupied(s, f.x, f.y));
    f.type = (FoodType)(rand() % FOOD_NUM);
    return f;
}

static void drawGame(SDL_Renderer* renderer, const Snake* s, const Food* f, TTF_Font* font, int score, int id) {
    SDL_SetRenderDrawColor(renderer, 20, 30, 40, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 100, 150, 200, 255);
    SDL_FRect border = { 0,0, GRID_W * CELL, GRID_H * CELL };
    SDL_RenderRect(renderer, &border);

    for (Node* cur = s->head; cur; cur = cur->next) {
        SDL_FRect r = { cur->x * CELL, cur->y * CELL, CELL, CELL };
        SDL_SetRenderDrawColor(renderer, id == 1 ? 0 : 100, 255, id == 1 ? 100 : 255, 255);
        SDL_RenderFillRect(renderer, &r);
    }
    SDL_FRect rf = { f->x * CELL, f->y * CELL, CELL, CELL };
    SDL_Color c = foodColor(f->type);
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(renderer, &rf);

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

void StartSinglePlayer(SDL_Renderer* renderer, TTF_Font* smallFont) {
    srand((unsigned)time(NULL));
    Snake* s = Snake_Create(GRID_W / 2, GRID_H / 2);
    Food f = newFood(s);
    int score = 0, high = 0;
    Uint32 last = SDL_GetTicks(), spd = 120;
    bool boost = false, run = true;
    Uint32 boostEnd = 0;

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
        if (now - last < (boost ? 60 : spd)) { SDL_Delay(1); continue; }
        last = now;

        Snake_Move(s);
        if (Snake_WallCollision(s) || Snake_SelfCollision(s)) break;
        SDL_Point h = Snake_HeadGrid(s);
        if (h.x == f.x && h.y == f.y) {
            score += (f.type == FOOD_BONUS ? 3 : 1);
            if (f.type == FOOD_SPEED) { boost = true; boostEnd = now + 5000; }
            Snake_Grow(s); f = newFood(s);
        }
        if (boost && now > boostEnd) boost = false;
        drawGame(renderer, s, &f, smallFont, score, 1);
    }

    GameOverData data = { 0 };
    data.finalScore = score;
    data.highScore = high;
    data.level = 1;
    data.length = 3;
    data.timeElapsed = SDL_GetTicks() / 1000;
    data.isNewHighScore = score > high;
    data.reason = GAME_OVER_SELF_COLLISION;
    showGameOverUI(renderer, NULL, smallFont, smallFont, &data);
    Snake_Destroy(s);
}

void StartMultiPlayer(SDL_Renderer* renderer, TTF_Font* smallFont) {
    srand((unsigned)time(NULL));
    Snake* s1 = Snake_Create(10, GRID_H / 2-10);
    Snake* s2 = Snake_Create(GRID_W - 10, GRID_H / 2 +10);
    s2->head->next->x = GRID_W - 9;   // 第二节右移 1
    s2->head->next->y = GRID_H / 2 + 10;
    s2->head->next->next->x = GRID_W - 8; // 第三节再右移 1
    s2->head->next->next->y = GRID_H / 2 + 10;
    s1->dir = DIR_RIGHT;
    s2->dir = DIR_LEFT;
    Food f = newFood(s1);
    int sc1 = 0, sc2 = 0;
    Uint32 last = SDL_GetTicks(), spd = 120;
    bool run = true;

    while (run) {
		//printf(">>>shaungren\n");//调试
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) { run = false; break; }
            if (e.type == SDL_EVENT_KEY_DOWN) {
                switch (e.key.scancode) {
                case SDL_SCANCODE_W: if (s1->dir != DIR_DOWN)  s1->dir = DIR_UP;    break;
                case SDL_SCANCODE_S: if (s1->dir != DIR_UP)    s1->dir = DIR_DOWN;  break;
                case SDL_SCANCODE_A: if (s1->dir != DIR_RIGHT) s1->dir = DIR_LEFT;  break;
                case SDL_SCANCODE_D: if (s1->dir != DIR_LEFT)  s1->dir = DIR_RIGHT; break;
                case SDL_SCANCODE_UP:    if (s2->dir != DIR_DOWN)  s2->dir = DIR_UP;    break;
                case SDL_SCANCODE_DOWN:  if (s2->dir != DIR_UP)    s2->dir = DIR_DOWN;  break;
                case SDL_SCANCODE_LEFT:  if (s2->dir != DIR_RIGHT) s2->dir = DIR_LEFT;  break;
                case SDL_SCANCODE_RIGHT: if (s2->dir != DIR_LEFT)  s2->dir = DIR_RIGHT; break;
                }
            }
        }
        Uint32 now = SDL_GetTicks();
        if (now - last < spd) { SDL_Delay(1); continue; }
        last = now;

        Snake_Move(s1); Snake_Move(s2);
        printf(">>> MoveDone s1=(%d,%d) s2=(%d,%d)\n",
            Snake_HeadGrid(s1).x, Snake_HeadGrid(s1).y,
            Snake_HeadGrid(s2).x, Snake_HeadGrid(s2).y);
        /* ---- 明细死亡检测 ---- */
        bool wall1 = Snake_WallCollision(s1), self1 = Snake_SelfCollision(s1), hit1 = HitOtherSnake(s1, s2);
        bool wall2 = Snake_WallCollision(s2), self2 = Snake_SelfCollision(s2), hit2 = HitOtherSnake(s2, s1);
        bool d1 = wall1 || self1 || hit1;
        bool d2 = wall2 || self2 || hit2;

        if (d1 || d2) {
            printf("=== DEATH d1=%d(w%d|s%d|h%d) d2=%d(w%d|s%d|h%d)\n",
                d1, wall1, self1, hit1, d2, wall2, self2, hit2);
            break;
        }
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

        for (Node* cur = s1->head; cur; cur = cur->next) {
            SDL_FRect r = { cur->x * CELL, cur->y * CELL, CELL, CELL };
            SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
            SDL_RenderFillRect(renderer, &r);
        }
        for (Node* cur = s2->head; cur; cur = cur->next) {
            SDL_FRect r = { cur->x * CELL, cur->y * CELL, CELL, CELL };
            SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255);
            SDL_RenderFillRect(renderer, &r);
        }
        SDL_FRect rf = { f.x * CELL, f.y * CELL, CELL, CELL };
        SDL_Color c = foodColor(f.type);
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_RenderFillRect(renderer, &rf);

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

    GameOverData data = { 0 };
    data.finalScore = sc1 + sc2;
    data.highScore = 0;
    data.level = 2;
    data.length = 3;
    data.timeElapsed = SDL_GetTicks() / 1000;
    data.isNewHighScore = false;
    data.reason = GAME_OVER_SELF_COLLISION;
    showGameOverUI(renderer, NULL, smallFont, smallFont, &data);
    Snake_Destroy(s1);
    Snake_Destroy(s2);
}