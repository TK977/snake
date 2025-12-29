#include "StartUI.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "gameOverUI.h"
#include <stdio.h>   // snprintf

#define PANEL_WIDTH  800   // 结算面板宽
#define PANEL_HEIGHT 700   // 结算面板高

/* ======== 颜色常量：背景、文字、高亮等 ======== */
static const SDL_Color COLOR_BG = { 20,  30,  40, 255 }; // 整个窗口背景
static const SDL_Color COLOR_PANEL_BG = { 30,  40,  50, 240 }; // 中心面板背景
static const SDL_Color COLOR_PANEL_BORDER = { 100, 150, 200, 255 }; // 面板边框
static const SDL_Color COLOR_TITLE_GAMEOVER = { 255,  80,  80, 255 }; // “Game Over”失败红
static const SDL_Color COLOR_TITLE_SUCCESS = { 80, 255, 120, 255 }; // 胜利绿（预留）
static const SDL_Color COLOR_TEXT_PRIMARY = { 255, 255, 255, 255 }; // 主要文字白
static const SDL_Color COLOR_TEXT_SECONDARY = { 180, 180, 180, 255 }; // 次要文字灰
static const SDL_Color COLOR_HIGHLIGHT = { 255, 215,   0, 255 }; // 分数金色高亮
static const SDL_Color COLOR_NEW_RECORD = { 255, 100, 255, 255 }; // “新纪录”粉色

/* 工具：居中渲染文字
 * renderer：SDL 渲染器
 * font：字体
 * text：字符串
 * centerX：屏幕绝对 X 中心
 * y：屏幕绝对 Y 坐标
 * color：文字颜色 */
static void renderTextCentered(SDL_Renderer* renderer, TTF_Font* font, const char* text,
    float centerX, float y, SDL_Color color) {
    if (!font || !text) return;
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, 0, color);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture) {
            SDL_FRect textRect;
            textRect.w = (float)textSurface->w;
            textRect.h = (float)textSurface->h;
            textRect.x = centerX - textRect.w / 2.0f;
            textRect.y = y;
            SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_DestroySurface(textSurface);
    }
}

/* 工具：左对齐渲染文字 */
static void drawTextLeft(SDL_Renderer* renderer, TTF_Font* font, const char* text,
    float x, float y, SDL_Color color) {
    if (!font || !text) return;
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, 0, color);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture) {
            SDL_FRect textRect;
            textRect.w = (float)textSurface->w;
            textRect.h = (float)textSurface->h;
            textRect.x = x;
            textRect.y = y;
            SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_DestroySurface(textSurface);
    }
}

/* 把秒数格式化成 MM:SS */
static void formatTime(int seconds, char* buffer, size_t bufferSize) {
    int minutes = seconds / 60;
    int secs = seconds % 60;
    snprintf(buffer, bufferSize, "%02d:%02d", minutes, secs);
}

/* 渲染半透明背景 + 圆角矩形面板 + 双边框 */
void renderGameOverBackground(SDL_Renderer* renderer) {
    // 1. 整个窗口背景
    SDL_SetRenderDrawColor(renderer, COLOR_BG.r, COLOR_BG.g, COLOR_BG.b, COLOR_BG.a);
    SDL_RenderClear(renderer);

    // 2. 中心面板位置
    int panelX = (WINDOW_WIDTH - PANEL_WIDTH) / 2;
    int panelY = (WINDOW_HEIGHT - PANEL_HEIGHT) / 2;
    SDL_FRect panelRect = { (float)panelX, (float)panelY,
                            (float)PANEL_WIDTH, (float)PANEL_HEIGHT };
    SDL_SetRenderDrawColor(renderer, COLOR_PANEL_BG.r, COLOR_PANEL_BG.g,
        COLOR_PANEL_BG.b, COLOR_PANEL_BG.a);
    SDL_RenderFillRect(renderer, &panelRect);

    // 3. 双边框装饰
    SDL_SetRenderDrawColor(renderer, COLOR_PANEL_BORDER.r, COLOR_PANEL_BORDER.g,
        COLOR_PANEL_BORDER.b, COLOR_PANEL_BORDER.a);
    SDL_FRect outerBorder = { panelX - 3, panelY - 3, PANEL_WIDTH + 6, PANEL_HEIGHT + 6 };
    SDL_RenderRect(renderer, &outerBorder);
    SDL_FRect innerBorder = { panelX - 1, panelY - 1, PANEL_WIDTH + 2, PANEL_HEIGHT + 2 };
    SDL_RenderRect(renderer, &innerBorder);
}

/* 渲染大标题（暂时写死“Game Over!!”，预留胜利标题） */
void drawTitle(SDL_Renderer* renderer, TTF_Font* font, char* text) {
    int centerX = WINDOW_WIDTH / 2;
    int titleY = (WINDOW_HEIGHT - PANEL_HEIGHT) / 2 + 50;

    SDL_Color titleColor = { 255,255,255,0 };  // 先占位白色，实际未使用

    renderTextCentered(renderer, font, "Game Over!!", (float)centerX, (float)titleY, titleColor);
}

/* “新纪录”动画：缩放 + 闪烁 */
void renderNewHighScoreAnima(SDL_Renderer* renderer, TTF_Font* font,
    int centerX, int y, Uint32 time) {
    // 1. 缩放因子：正弦波动 1.0~1.1
    float scale = 1.0f + 0.1f * sinf(time * 0.005f);

    // 2. 颜色 alpha 闪烁：200~255
    Uint8 alpha = (Uint8)(200 + 55 * sinf(time * 0.003f));
    SDL_Color animColor = COLOR_NEW_RECORD;
    animColor.a = alpha;
    const char* text = " NEW HIGH SCORE! ";

    SDL_Surface* surface = TTF_RenderText_Blended(font, text, 0, animColor);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            int scaledW = (int)(surface->w * scale);
            int scaledH = (int)(surface->h * scale);
            SDL_FRect rect = { centerX - scaledW / 2.0f, y, scaledW, scaledH };
            SDL_RenderTexture(renderer, texture, NULL, &rect);
            SDL_DestroyTexture(texture);
        }
        SDL_DestroySurface(surface);
    }
}

/* 渲染分数、等级、长度、时间等详细数据 */
void renderGameOverStats(SDL_Renderer* renderer, TTF_Font* font, GameOverData* data) {
    int centerX = WINDOW_WIDTH / 2;
    int statsY = (WINDOW_HEIGHT - PANEL_HEIGHT) / 2 + 200; // 第一行基准 Y
    int lineHeight = 50;
    char buffer[128];

    // 1. 判断是否是新纪录
    if (data->finalScore > data->highScore) {
        data->isNewHighScore = true;
    }

    // 2. 如果是新纪录，先渲染动画
    if (data->isNewHighScore) {
        renderNewHighScoreAnima(renderer, font, centerX,
            statsY - 60, SDL_GetTicks());
    }

    // 3. 最终分数
    renderTextCentered(renderer, font, "Final Score", centerX, statsY, COLOR_TEXT_SECONDARY);
    snprintf(buffer, sizeof(buffer), "%d", data->finalScore);
    renderTextCentered(renderer, font, buffer, centerX, statsY + 35, COLOR_HIGHLIGHT);

    statsY += lineHeight + 50;

    // 4. 分隔线
    SDL_SetRenderDrawColor(renderer, 80, 90, 100, 255);
    SDL_RenderLine(renderer,
        centerX - 300, statsY,
        centerX + 300, statsY);

    statsY += 30;

    // 5. 左右两列详细信息
    int leftX = centerX - 250;
    int rightX = centerX + 50;

    // 左列
    drawTextLeft(renderer, font, "High Score:", leftX, statsY, COLOR_TEXT_SECONDARY);
    snprintf(buffer, sizeof(buffer), "%d", data->highScore);
    drawTextLeft(renderer, font, buffer, leftX + 180, statsY, COLOR_TEXT_PRIMARY);

    drawTextLeft(renderer, font, "Level:", leftX, statsY + 45, COLOR_TEXT_SECONDARY);
    snprintf(buffer, sizeof(buffer), "%d", data->level);
    drawTextLeft(renderer, font, buffer, leftX + 180, statsY + 45, COLOR_TEXT_PRIMARY);

    // 右列
    drawTextLeft(renderer, font, "Length:", rightX, statsY, COLOR_TEXT_SECONDARY);
    snprintf(buffer, sizeof(buffer), "%d", data->length);
    drawTextLeft(renderer, font, buffer, rightX + 180, statsY, COLOR_TEXT_PRIMARY);

    drawTextLeft(renderer, font, "Time:", rightX, statsY + 45, COLOR_TEXT_SECONDARY);
    formatTime(data->timeElapsed, buffer, sizeof(buffer));
    drawTextLeft(renderer, font, buffer, rightX + 180, statsY + 45, COLOR_TEXT_PRIMARY);
}

/* 外部按钮渲染函数声明（在 StartUI.c 实现）*/
extern void renderButton(SDL_Renderer* renderer, Button* btn, TTF_Font* font);
extern bool isPointInButton(Button* btn, float x, float y);

/* 游戏结束主界面：负责事件循环、渐变、按钮交互
 * 返回枚举：重启 / 回主菜单 / 退出游戏 */
GameOverAction showGameOverUI(SDL_Renderer* renderer, TTF_Font* titleFont,
    TTF_Font* buttonFont, TTF_Font* smallFont, GameOverData* data) {

    // 1. 创建三个按钮：Restart / Main Menu / Exit Game
    int centerX = WINDOW_WIDTH / 2;
    int buttonY = (WINDOW_HEIGHT + PANEL_HEIGHT) / 2 - 150;
    int buttonWidth = 250;
    int buttonHeight = 60;
    int buttonSpacing = 20;

    Button buttons[3];
    buttons[0] = create_button(
        centerX - buttonWidth - buttonSpacing / 2,
        buttonY,
        buttonWidth,
        buttonHeight,
        "Restart"
    );

    buttons[1] = create_button(
        centerX + buttonSpacing / 2,
        buttonY,
        buttonWidth,
        buttonHeight,
        "Main Menu"
    );

    buttons[2] = create_button(
        centerX - buttonWidth / 2,
        buttonY + buttonHeight + buttonSpacing,
        buttonWidth,
        buttonHeight,
        "Exit Game"
    );

    bool running = true;
    SDL_Event event;
    GameOverAction action = GAMEOVER_MENU; // 默认返回主菜单

    // 2. 淡入透明度过渡
    Uint8 fadeAlpha = 0;
    bool fadingIn = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                action = GAMEOVER_EXIT;
                running = false;
            }
            else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                float mouseX = event.motion.x;
                float mouseY = event.motion.y;
                for (int i = 0; i < 3; i++) {
                    buttons[i].isHovered = isPointInButton(&buttons[i], mouseX, mouseY);
                }
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float mouseX = event.button.x;
                float mouseY = event.button.y;
                for (int i = 0; i < 3; i++) {
                    buttons[i].isPressed = isPointInButton(&buttons[i], mouseX, mouseY);
                }
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                for (int i = 0; i < 3; i++) {
                    if (buttons[i].isPressed && buttons[i].isHovered) {
                        switch (i) {
                        case 0:
                            action = GAMEOVER_RESTART;
                            running = false;
                            break;
                        case 1:
                            action = GAMEOVER_MENU;
                            running = false;
                            break;
                        case 2:
                            action = GAMEOVER_EXIT;
                            running = false;
                            break;
                        }
                    }
                    buttons[i].isPressed = false;
                }
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) {
                    action = GAMEOVER_MENU;
                    running = false;
                }
                else if (event.key.key == SDLK_SPACE || event.key.key == SDLK_RETURN) {
                    action = GAMEOVER_RESTART; // 空格/回车 默认重启
                    running = false;
                }
            }
        }

        // 3. 每帧更新淡入
        if (fadingIn) {
            fadeAlpha += 5;
            if (fadeAlpha >= 255) {
                fadeAlpha = 255;
                fadingIn = false;
            }
        }

        // 4. 渲染
        renderGameOverBackground(renderer);
        renderGameOverStats(renderer, buttonFont, data); // 分数等
        drawTitle(renderer, titleFont, "GameOver!!");    // 大标题

        for (int i = 0; i < 3; i++) {
            renderButton(renderer, &buttons[i], buttonFont);
        }

        // 5. 底部提示文字
        renderTextCentered(renderer, smallFont,
            "Press ENTER to restart or ESC to return to menu",
            WINDOW_WIDTH / 2,
            WINDOW_HEIGHT - 50,
            COLOR_TEXT_SECONDARY);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // 约 60 FPS
    }

    return action;
}