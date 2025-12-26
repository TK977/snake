#include "SettingsUI.h"
#include "Settings.h"
#include "StartUI.h"
#include "Data.h"
#include <string.h>

// 设置颜色
static const SDL_Color COLORS[] = {
    { 0, 255, 100, 255 },   // 绿
    { 100, 200, 255, 255 }, // 蓝
    { 255, 100, 100, 255 }, // 红
    { 255, 215, 0, 255 },   // 黄
    { 200, 100, 255, 255 }  // 紫
};
static const int COLOR_COUNT = 5;

void ShowSettingsUI(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* font, TTF_Font* smallFont) {
    bool running = true;
    SDL_Event event;

    char originalName[30];
    strcpy_s(originalName, sizeof(originalName), currentSettings.nickname);

    Button backButton = create_button(50, 50, 100, 50, "Back");
    
	// 设置颜色选择区域
    SDL_FRect colorRects[5];
    float startX = 400;
    float startY = 500;
    float size = 60;
    float gap = 20;
    
    for (int i = 0; i < COLOR_COUNT; i++) {
        colorRects[i] = (SDL_FRect){ startX + i * (size + gap), startY, size, size };
    }

    
    // 这里是为了设置昵称而启用文本输入
    SDL_StartTextInput(win); 

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;

				// 如果用户选择了退出，终止“设置”操作，让主循环处理退出
				SDL_PushEvent(&event); // 把event重新放回事件队列，让startUI处理退出
                running = false;
            }
            // 文本输入处理
            else if (event.type == SDL_EVENT_TEXT_INPUT) {
                if (strlen(currentSettings.nickname) + strlen(event.text.text) < 30) {
                    strcat_s(currentSettings.nickname, sizeof(currentSettings.nickname), event.text.text);
                }
            }
			// 如果按下退格键，删除最后一个字符
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_BACKSPACE) {
                    size_t len = strlen(currentSettings.nickname);
                    if (len > 0) {
                        currentSettings.nickname[len - 1] = '\0';
                    }
                }
            }
            // 处理鼠标移动
            else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                float mx = event.motion.x;
                float my = event.motion.y;
                backButton.isHovered = isPointInButton(&backButton, mx, my);
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float mx = event.button.x;
                float my = event.button.y;
                
                if (isPointInButton(&backButton, mx, my)) {
                    backButton.isPressed = true;
                    running = false;
                    UpdatePlayerName(originalName, currentSettings.nickname);
                }

                // 选择颜色。不同的位置对应不同颜色
                for (int i = 0; i < COLOR_COUNT; i++) {
                    if (mx >= colorRects[i].x && mx <= colorRects[i].x + colorRects[i].w &&
                        my >= colorRects[i].y && my <= colorRects[i].y + colorRects[i].h) {
                        SetSnakeColor(COLORS[i]);
                    }
                }
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                backButton.isPressed = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        renderTitle(renderer, titleFont, "Settings", 100);

        // 渲染昵称标签
        SDL_Color white = { 255, 255, 255, 255 };
        SDL_Surface* surf = TTF_RenderText_Blended(font, "Nickname:", 0, white);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FRect dst = { 400, 300, (float)surf->w, (float)surf->h };
            SDL_RenderTexture(renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
            SDL_DestroySurface(surf);
        }

        // 渲染现有的昵称（白色，在框里）
        if (strlen(currentSettings.nickname) > 0) {
            surf = TTF_RenderText_Blended(font, currentSettings.nickname, 0, white);
        } else {
             surf = TTF_RenderText_Blended(font, " ", 0, white); // Empty string placeholder
        }
        
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FRect dst = { 600, 300, (float)surf->w, (float)surf->h };
            SDL_RenderTexture(renderer, tex, NULL, &dst);
            
            // 渲染昵称框下面的下划线
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_FRect line = { 600, 300 + surf->h + 5, 300, 2 };
            SDL_RenderFillRect(renderer, &line);

            SDL_DestroyTexture(tex);
            SDL_DestroySurface(surf);
        }


        // 渲染下面的颜色选择模块
        surf = TTF_RenderText_Blended(font, "Snake Color:", 0, white);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FRect dst = { 400, 450, (float)surf->w, (float)surf->h };
            SDL_RenderTexture(renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
            SDL_DestroySurface(surf);
        }

        for (int i = 0; i < COLOR_COUNT; i++) {
            SDL_SetRenderDrawColor(renderer, COLORS[i].r, COLORS[i].g, COLORS[i].b, COLORS[i].a);
            SDL_RenderFillRect(renderer, &colorRects[i]);

            // 对选中的框做高亮处理
            if (currentSettings.snakeColor.r == COLORS[i].r &&
                currentSettings.snakeColor.g == COLORS[i].g &&
                currentSettings.snakeColor.b == COLORS[i].b) {
                
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_FRect border = { colorRects[i].x - 4, colorRects[i].y - 4, colorRects[i].w + 8, colorRects[i].h + 8 };
                SDL_RenderRect(renderer, &border);
            }
        }


        renderButton(renderer, &backButton, font);

        SDL_RenderPresent(renderer);
    }

    SDL_StopTextInput(win);
}
