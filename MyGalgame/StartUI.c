//主要完成初始页面的渲染创建
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include "inifunc.h"
#include "StartUI.h"
#include "LookRankings.h"
#include "GamePlay.h"
#include "SettingsUI.h"




SDL_Window* win = NULL;

//创建按钮函数
Button create_button(float x, float y, float w, float h, const char* text) {
    Button btn = {
        .rect = {x, y, w, h},
        .text = text,
        .hoverColor = {200, 200, 200, 255},
        .normalColor = {255, 255, 255, 255},
        .pressedColor = {150, 150, 150, 255},
        .textColor = {0, 0, 0, 255},
        .isHovered = false,
        .isPressed = false,
        .isEnabled = true
    };
    return btn;
}
//判断鼠标是否在按钮内函数
bool isPointInButton(Button* btn, float x, float y) {
    if (!btn->isEnabled) {
        //按钮不可用
        return false;
    }
    else {
        if (x >= btn->rect.x && x <= btn->rect.x + btn->rect.w &&
            y >= btn->rect.y && y <= btn->rect.y + btn->rect.h) {
            return true;
        }
        else {
            return false;
        }
    }
}
//渲染标题函数
void renderTitle(SDL_Renderer* renderer, TTF_Font* font, const char* text, float y) {
    if (!font || !text) return;
    SDL_Color titleColor = { 255, 215, 0, 255 }; // 金色标题

    // 创建文本表面
    SDL_Surface* titleSurface = TTF_RenderText_Blended(font, text, 0, titleColor);

    if (titleSurface) {
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);

        if (titleTexture) {
            SDL_FRect titleRect;
            titleRect.w = (float)titleSurface->w;
            titleRect.h = (float)titleSurface->h;
            titleRect.x = (WINDOW_WIDTH - titleRect.w) / 2.0f; // 居中
            titleRect.y = y;

            SDL_RenderTexture(renderer, titleTexture, NULL, &titleRect);
            SDL_DestroyTexture(titleTexture);
        }
        SDL_DestroySurface(titleSurface);
    }
}

//渲染按钮函数
void renderButton(SDL_Renderer* renderer, Button* btn, TTF_Font* font) {
    if (!btn || !renderer) return;

    // Determine fill color
    SDL_Color fillColor;
    if (!btn->isEnabled) {
        fillColor = (SDL_Color){ 60, 60, 60, 150 };
    }
    else if (btn->isPressed) {
        fillColor = btn->pressedColor;
    }
    else if (btn->isHovered) {
        fillColor = btn->hoverColor;
    }
    else {
        fillColor = btn->normalColor;
    }

    SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    SDL_RenderFillRect(renderer, &btn->rect);

    // 绘制按钮边框
    if (btn->isHovered && btn->isEnabled) {
        SDL_SetRenderDrawColor(renderer, 150, 150, 200, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 120, 200);
    }
    SDL_RenderRect(renderer, &btn->rect);

    // 渲染按钮文本
    SDL_Color textColor = btn->isEnabled ? btn->textColor : (SDL_Color) { 100, 100, 100, 255 };

    if (font && btn->text) {
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, btn->text, 0, textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_FRect textRect;
                textRect.w = (float)textSurface->w;
                textRect.h = (float)textSurface->h;
                textRect.x = btn->rect.x + (btn->rect.w - textRect.w) / 2.0f;
                textRect.y = btn->rect.y + (btn->rect.h - textRect.h) / 2.0f;

                SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_DestroySurface(textSurface);
        }
    }
}

 void StartUICreate() {
    if (!initSDLsubsystem()) return;

    //创建窗口
    //SDL_Window* win = NULL;
    win = SDL_CreateWindow("The Snake", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (win == NULL) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        uninitSDLsubsystem();
        return;
    }

    // 创建渲染器
    SDL_Renderer* renderer = SDL_CreateRenderer(win, NULL);
    if (!renderer) {
        SDL_Log("Renderer Create Failed: %s", SDL_GetError());
        SDL_DestroyWindow(win);
        uninitSDLsubsystem();
        return;
    }

    // enable blending
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // 尝试以相对路径加载字体，失败则尝试项目路径
    TTF_Font* titleFont = TTF_OpenFont("C:\\snakegamep\\MyGalgame\\x64\\Debug\\font.ttf", 96);
    TTF_Font* buttonFont = TTF_OpenFont("C:\\snakegamep\\MyGalgame\\x64\\Debug\\font.ttf", 28);
    TTF_Font* smallFont = TTF_OpenFont("C:\\snakegamep\\MyGalgame\\x64\\Debug\\font.ttf", 16);
    if (!titleFont || !buttonFont || !smallFont) {
        // 尝试备用路径（项目调试目录）
        const char* fallback = "C:\\Users\\wtk\\Desktop\\MyGalgame\\x64\\Debug\\font.ttf";
        if (!titleFont) titleFont = TTF_OpenFont(fallback, 96);
        if (!buttonFont) buttonFont = TTF_OpenFont(fallback, 28);
        if (!smallFont) smallFont = TTF_OpenFont(fallback, 16);
    }

    if (!titleFont || !buttonFont || !smallFont) {
        SDL_Log("Font Loading Failed.");
        if (titleFont) TTF_CloseFont(titleFont);
        if (buttonFont) TTF_CloseFont(buttonFont);
        if (smallFont) TTF_CloseFont(smallFont);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(win);
        uninitSDLsubsystem();
        return;
    }

    float centerX = WINDOW_WIDTH / 2.0f;
    float buttonWidth = 300;
    float buttonHeight = 60;
    float startY = 300;
    float spacing = 80;
    //创建5个按钮
    Button buttons[5];
    buttons[0] = create_button(centerX - buttonWidth / 2, startY + spacing * 0,
        buttonWidth, buttonHeight, "Single Mode");
    buttons[1] = create_button(centerX - buttonWidth / 2, startY + spacing * 1,
        buttonWidth, buttonHeight, "Muti Mode");
    buttons[2] = create_button(centerX - buttonWidth / 2, startY + spacing * 2,
        buttonWidth, buttonHeight, "Rankings");
    buttons[3] = create_button(centerX - buttonWidth / 2, startY + spacing * 3,
        buttonWidth, buttonHeight, "Settings");
    buttons[4] = create_button(centerX - buttonWidth / 2, startY + spacing * 4,
        buttonWidth, buttonHeight, "Exit");

    // 窗口保持,等待用户关闭
    bool flag = true;
    SDL_Event event; //事件监听
    while (flag) {
        if (SDL_WaitEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                break;
            }
            else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                //获取鼠标坐标
                float mouseX = event.motion.x;
                float mouseY = event.motion.y;
                for (int i = 0; i < 5; i++) {
                    buttons[i].isHovered = isPointInButton(&buttons[i], mouseX, mouseY);
                }
            }
            //处理鼠标按下的情况
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float mouseX = event.button.x;
                float mouseY = event.button.y;
                for (int i = 0; i < 5; i++) {
                    if (isPointInButton(&buttons[i], mouseX, mouseY)) {
                        buttons[i].isPressed = true;
                        //这个位置要有接口，开始游戏要进入游戏界面
                        switch (i) {
                        case 0: {   // 单人——支持 Restart
                            do {
                                StartSinglePlayer(renderer, smallFont);
                                /* 后续把 StartSinglePlayer 改成返回 GameOverAction 即可重开 */
                            } while (false);   // 只玩一次，先不循环
                            buttons[0].isPressed = false;
                            break;
                        }
                        case 1: {   // 双人——支持 Restart
                            GameOverAction act;
                            do {
                                act = StartMultiPlayer(renderer, titleFont, buttonFont, smallFont);
                            } while (act == GAMEOVER_RESTART);
                            buttons[1].isPressed = false;
                            break;
                        }
                        case 2:
                            SDL_Log("Rankings");
                            LookRankings(renderer, titleFont, buttonFont, smallFont);
                            buttons[2].isPressed = false; // 返回后重置按钮状态
                            break;
                        case 3:
                            SDL_Log("Settings");
                            ShowSettingsUI(renderer, titleFont, buttonFont, smallFont);
                            buttons[3].isPressed = false;
                            break;
                        case 4:
                            SDL_Log("Exit");
                            flag = false;
                            break;
                        }
                    }
                }
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                for (int i = 0; i < 5; i++) {
                    buttons[i].isPressed = false;
                }
            }
        }

        // 清理背景
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        //渲染标题
        if (titleFont) {
            renderTitle(renderer, titleFont, "The Snake", 100);
        }
        //渲染按钮
        for (int i = 0; i < 5; i++) {
            renderButton(renderer, &buttons[i], buttonFont);
        }
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // 约60 FPS
    }

    // 清理字体与渲染器
    TTF_CloseFont(titleFont);
    TTF_CloseFont(buttonFont);
    TTF_CloseFont(smallFont);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    uninitSDLsubsystem();
}