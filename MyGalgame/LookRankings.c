#define _CRT_SECURE_NO_WARNINGS

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "StartUI.h"
#include <stdbool.h>
#include <stdio.h>
#include "LookRankings.h"
#include <string.h>


//渲染排行榜标题

void renderRankingTitle(SDL_Renderer* renderer, TTF_Font* font, const char* text, float y) {
    SDL_Color titleColor = { 100, 200, 255, 255 }; // 蓝色标题

    SDL_Surface* titleSurface = TTF_RenderText_Blended(font, text, 0, titleColor);

    if (titleSurface) {
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);

        if (titleTexture) {
            SDL_FRect titleRect;
            titleRect.w = titleSurface->w;
            titleRect.h = titleSurface->h;
            titleRect.x = (WINDOW_WIDTH - titleRect.w) / 2;
            titleRect.y = y;

            SDL_RenderTexture(renderer, titleTexture, NULL, &titleRect);
            SDL_DestroyTexture(titleTexture);
        }
        SDL_DestroySurface(titleSurface);
    }
}

//渲染排行榜条目
void renderRankingEntry(SDL_Renderer* renderer, TTF_Font* font, RankingEntry* entry, float y, bool isHovered) {
    char EntryText[128];

    // 格式化排行榜文本：排名. 姓名 - 分数
    snprintf(EntryText, sizeof(EntryText), "%d.  %-20s  %d", entry->rank, entry->name, entry->score);
    SDL_Color TextColor;
    if (entry->rank == 1) {
        TextColor = (SDL_Color){ 255, 215, 0, 255 }; // 金色
    }
    else if (entry->rank == 2) {
        TextColor = (SDL_Color){ 192, 192, 192, 255 };//银色
    }
    else if (entry->rank == 3) {
        TextColor = (SDL_Color){ 205, 127, 50, 255 };//铜色
    }
    else {
        TextColor = (SDL_Color){ 220, 220, 220, 255 };//白色
    }
    if (isHovered) {
        SDL_FRect highlightRect = { WINDOW_WIDTH / 2 - 400,y - 5,800,50 };
        SDL_SetRenderDrawColor(renderer, 60, 80, 100, 100);
        SDL_RenderFillRect(renderer, &highlightRect);
    }
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, EntryText, 0, TextColor);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        if (textTexture) {
            SDL_FRect textRect;
            textRect.w = textSurface->w;
            textRect.h = textSurface->h;
            textRect.x = WINDOW_WIDTH / 2 - textRect.w / 2;
            textRect.y = y;
            SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }
    }
}


//渲染表格

void RenderHead(SDL_Renderer* renderer, TTF_Font* font, float y) {
    SDL_Color headerColor = { 150,150,150,255 };
    const char* headerText = "Rank    Name                      Score";
    SDL_Surface* headerSurface = TTF_RenderText_Blended(font, headerText, 0, headerColor);
    if (headerSurface) {
        SDL_Texture* headerTexture = SDL_CreateTextureFromSurface(renderer, headerSurface);
        if (headerTexture) {
            SDL_FRect headerRect;
            headerRect.w = headerSurface->w;
            headerRect.h = headerSurface->h;
            headerRect.x = WINDOW_WIDTH / 2 - headerRect.w / 2;
            headerRect.y = y;
            SDL_RenderTexture(renderer, headerTexture, NULL, &headerRect);
            SDL_DestroyTexture(headerTexture);

            //分割线
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
            SDL_RenderLine(renderer,
                WINDOW_WIDTH / 2 - 400, y + headerRect.h + 5,
                WINDOW_WIDTH / 2 + 400, y + headerRect.h + 5);
        }
        SDL_DestroySurface(headerSurface);
    }
}

//从文件加载排行榜数据

int LoadData(RankingEntry* Rankings, int MaxEntries) {
    FILE* file = fopen("rankings.dat", "r");
    int count = 0;
    //文件不存在
    if (file == NULL) {
        SDL_Log("FileDoesNotExist");
        strcpy(Rankings[0].name, "Player1");
        Rankings[0].score = 1000;
        Rankings[0].rank = 1;

        strcpy(Rankings[1].name, "Player2");
        Rankings[1].score = 850;
        Rankings[1].rank = 2;

        strcpy(Rankings[2].name, "Player3");
        Rankings[2].score = 720;
        Rankings[2].rank = 3;

        strcpy(Rankings[3].name, "Player4");
        Rankings[3].score = 650;
        Rankings[3].rank = 4;

        strcpy(Rankings[4].name, "Player5");
        Rankings[4].score = 580;
        Rankings[4].rank = 5;

        strcpy(Rankings[5].name, "Player6");
        Rankings[5].score = 500;
        Rankings[5].rank = 6;

        strcpy(Rankings[6].name, "Player7");
        Rankings[6].score = 420;
        Rankings[6].rank = 7;

        strcpy(Rankings[7].name, "Player8");
        Rankings[7].score = 350;
        Rankings[7].rank = 8;

        strcpy(Rankings[8].name, "Player9");
        Rankings[8].score = 280;
        Rankings[8].rank = 9;

        strcpy(Rankings[9].name, "Player10");
        Rankings[9].score = 200;
        Rankings[9].rank = 10;

        return 10;
    }
    else {
        while (count < MaxEntries && fscanf(file, "%d %s %d",
            &Rankings[count].rank,
            Rankings[count].name,
            &Rankings[count].score) == 3) {
            count++;
        }
    }
    fclose(file);
    return count;
}

void LookRankings(SDL_Renderer* renderer, TTF_Font* TitleFont, TTF_Font* NormallFont, TTF_Font* SmallFont) {
    RankingEntry entry[50];
    int cnt = LoadData(entry, 50);

    Button backToMenuBtn = create_button(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT - 120, 300, 60, "Back to Menu");
    /*
    // 滚动相关变量
    int scrollOffset = 0;
    int maxScroll = (cnt > 10) ? (cnt - 10) * 55 : 0;
    int hoveredIndex = -1; // 当前鼠标悬停的排行榜条目
    */

    bool running = true;
    SDL_Event event;
    while (running) {
        if (SDL_PollEvent(&event)) {//当前有事件发生
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                float mouseX = event.motion.x;
                float mouseY = event.motion.y;
                backToMenuBtn.isHovered = isPointInButton(&backToMenuBtn, mouseX, mouseY);
                /*
                hoveredIndex = -1;
                float startY = 280;
                for (int i = 0; i < rankingCount && i < 10; i++) {
                    float entryY = startY + i * 55 - scrollOffset;
                    if (mouseY >= entryY && mouseY <= entryY + 50) {
                        hoveredIndex = i;
                        break;*/
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float mouseX = event.button.x;
                float mouseY = event.button.y;
                backToMenuBtn.isPressed = isPointInButton(&backToMenuBtn, mouseX, mouseY);
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                float mouseX = event.button.x;
                float mouseY = event.button.y;
                if (backToMenuBtn.isPressed && backToMenuBtn.isHovered) {
                    running = false;
                }
                backToMenuBtn.isPressed = false;
            }/*
            else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                // 鼠标滚轮支持
                scrollOffset -= event.wheel.y * 30;
                if (scrollOffset < 0) scrollOffset = 0;
                if (scrollOffset > maxScroll) scrollOffset = maxScroll;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                // 键盘控制滚动
                if (event.key.key == SDLK_UP) {
                    scrollOffset -= 55;
                    if (scrollOffset < 0) scrollOffset = 0;
                }
                else if (event.key.key == SDLK_DOWN) {
                    scrollOffset += 55;
                    if (scrollOffset > maxScroll) scrollOffset = maxScroll;
                }
                else if (event.key.key == SDLK_ESCAPE) {
                    running = false;
                }
             }*/
        }
        //所有事件处理完毕，开始渲染
        //清屏
        SDL_SetRenderDrawColor(renderer, 20, 30, 40, 255);
        SDL_RenderClear(renderer);
        //渲染标题
        renderRankingTitle(renderer, TitleFont, "Rankings", 80);
        //渲染表格
        RenderHead(renderer, NormallFont, 200);
        //渲染排行榜条目
        float startY = 280;
        for (int i = 0; i < cnt; i++) {
            float entryY = startY + i * 55;
            bool isHovered = false;
            float mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            if (mouseY >= entryY && mouseY <= entryY + 50 &&
                mouseX >= WINDOW_WIDTH / 2 - 400 && mouseX <= WINDOW_WIDTH / 2 + 400) {
                isHovered = true;
            }
            renderRankingEntry(renderer, NormallFont, &entry[i], entryY, isHovered);
        }
        //渲染返回按钮
        renderButton(renderer, &backToMenuBtn, SmallFont);
        //更新屏幕
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // 大约60 FPS
    }
}