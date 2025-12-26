#include "MultiGameOverUI.h"
#include "StartUI.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>

// Helper to render centered text
static void RenderCenteredText(SDL_Renderer* renderer, TTF_Font* font, const char* text, float y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, 0, color);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_FRect dst = { 
                (float)(WINDOW_WIDTH - surface->w) / 2.0f, 
                y, 
                (float)surface->w, 
                (float)surface->h 
            };
            SDL_RenderTexture(renderer, texture, NULL, &dst);
            SDL_DestroyTexture(texture);
        }
        SDL_DestroySurface(surface);
    }
}

MultiPlayerGameOverAction showMultiPlayerGameOverUI(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* buttonFont, TTF_Font* smallFont, MultiPlayerGameOverData* data) {
    bool running = true;
    MultiPlayerGameOverAction action = MP_GAMEOVER_MENU;
    SDL_Event event;

    // Define buttons
    float centerX = WINDOW_WIDTH / 2.0f;
    float startY = WINDOW_HEIGHT / 2.0f + 150;
    float btnWidth = 200;
    float btnHeight = 60;
    float spacing = 20;

    Button restartBtn = create_button(centerX - btnWidth - spacing, startY, btnWidth, btnHeight, "Rematch");
    Button menuBtn = create_button(centerX + spacing, startY, btnWidth, btnHeight, "Menu");

    char titleText[64];
    SDL_Color titleColor;

    if (data->reason == MP_GAME_OVER_PLAYER1_WIN) {
        snprintf(titleText, sizeof(titleText), "%s Wins!", data->player1.name);
        titleColor = data->player1.color;
    } else if (data->reason == MP_GAME_OVER_PLAYER2_WIN) {
        snprintf(titleText, sizeof(titleText), "%s Wins!", data->player2.name);
        titleColor = data->player2.color;
    } else {
        snprintf(titleText, sizeof(titleText), "Draw!");
        titleColor = (SDL_Color){200, 200, 200, 255};
    }

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
                action = MP_GAMEOVER_EXIT;
            }
            else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                float mx = event.motion.x;
                float my = event.motion.y;
                restartBtn.isHovered = isPointInButton(&restartBtn, mx, my);
                menuBtn.isHovered = isPointInButton(&menuBtn, mx, my);
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float mx = event.button.x;
                float my = event.button.y;
                if (isPointInButton(&restartBtn, mx, my)) {
                    restartBtn.isPressed = true;
                    action = MP_GAMEOVER_REMATCH;
                    running = false;
                }
                if (isPointInButton(&menuBtn, mx, my)) {
                    menuBtn.isPressed = true;
                    action = MP_GAMEOVER_MENU;
                    running = false;
                }
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                restartBtn.isPressed = false;
                menuBtn.isPressed = false;
            }
        }

        // --- RENDER ---
        // Semi-transparent background overlay
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); // Dark overlay
        SDL_FRect fullscreen = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        SDL_RenderFillRect(renderer, &fullscreen);

        // Title
        RenderCenteredText(renderer, titleFont, titleText, 200, titleColor);

        // Stats
        char statsBuf[128];
        snprintf(statsBuf, sizeof(statsBuf), "Score: %d - %d", data->player1.score, data->player2.score);
        RenderCenteredText(renderer, buttonFont, statsBuf, 350, (SDL_Color){255, 255, 255, 255});

        snprintf(statsBuf, sizeof(statsBuf), "Time: %ds", data->totalTime);
        RenderCenteredText(renderer, smallFont, statsBuf, 420, (SDL_Color){200, 200, 200, 255});

        // Buttons
        renderButton(renderer, &restartBtn, buttonFont);
        renderButton(renderer, &menuBtn, buttonFont);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    return action;
}