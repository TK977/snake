#include "StartUI.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "gameOverUI.h"
#include <stdio.h>   // snprintf

#define PANEL_WIDTH 800
#define PANEL_HEIGHT 700

// 设置需要的颜色
static const SDL_Color COLOR_BG = { 20, 30, 40, 255 };
static const SDL_Color COLOR_PANEL_BG = { 30, 40, 50, 240 };
static const SDL_Color COLOR_PANEL_BORDER = { 100, 150, 200, 255 };
static const SDL_Color COLOR_TITLE_GAMEOVER = { 255, 80, 80, 255 };
static const SDL_Color COLOR_TITLE_SUCCESS = { 80, 255, 120, 255 };
static const SDL_Color COLOR_TEXT_PRIMARY = { 255, 255, 255, 255 };
static const SDL_Color COLOR_TEXT_SECONDARY = { 180, 180, 180, 255 };
static const SDL_Color COLOR_HIGHLIGHT = { 255, 215, 0, 255 };
static const SDL_Color COLOR_NEW_RECORD = { 255, 100, 255, 255 };


// 居中渲染文字的函数
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
// 左对齐渲染文字的函数
static drawTextLeft(SDL_Renderer* renderer, TTF_Font* font, const char* text, float x, float y, SDL_Color color) {
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

// 格式化时间为MM:SS格式的函数
static void formatTime(int seconds, char* buffer, size_t bufferSize) {
	int minutes = seconds / 60;
	int secs = seconds % 60;
	snprintf(buffer, bufferSize, "%02d:%02d", minutes, secs);
}

void renderGameOverBackground(SDL_Renderer* renderer) {
	//根据背景颜色设置渲染
	SDL_SetRenderDrawColor(renderer, COLOR_BG.r, COLOR_BG.g, COLOR_BG.b, COLOR_BG.a);
	SDL_RenderClear(renderer);

	int panelX = (WINDOW_WIDTH - PANEL_WIDTH) / 2;
	int panelY = (WINDOW_HEIGHT - PANEL_HEIGHT) / 2;

	SDL_FRect panelRect = { (float)panelX, (float)panelY, (float)PANEL_WIDTH, (float)PANEL_HEIGHT };
	SDL_SetRenderDrawColor(renderer, COLOR_PANEL_BG.r, COLOR_PANEL_BG.g, COLOR_PANEL_BG.b, COLOR_PANEL_BG.a);
	SDL_RenderFillRect(renderer, &panelRect);

	SDL_SetRenderDrawColor(renderer, COLOR_PANEL_BORDER.r, COLOR_PANEL_BORDER.g,
		COLOR_PANEL_BORDER.b, COLOR_PANEL_BORDER.a);
	SDL_FRect outerBorder = { panelX - 3, panelY - 3, PANEL_WIDTH + 6, PANEL_HEIGHT + 6 };
	SDL_RenderRect(renderer, &outerBorder);

	SDL_FRect innerBorder = { panelX - 1, panelY - 1, PANEL_WIDTH + 2, PANEL_HEIGHT + 2 };
	SDL_RenderRect(renderer, &innerBorder);
}

//渲染标题。特别注意，标题内容可能是游戏结束或者游戏胜利
//失败为红色字体，胜利为绿色字体
void drawTitle(SDL_Renderer* renderer, TTF_Font* font, char* text) {
	int centerX = WINDOW_WIDTH / 2;
	int titleY = (WINDOW_HEIGHT - PANEL_HEIGHT) / 2 + 50;

	SDL_Color titleColor = { 255,255,255,0 };  //�Ȱѱ�����ɰ׵�

	renderTextCentered(renderer, font, "Game Over!!", (float)centerX, (float)titleY, titleColor);
}


//新纪录的动画
void renderNewHighScoreAnima(SDL_Renderer* renderer, TTF_Font* font,
	int centerX, int y, Uint32 time) {
	// 缩放效果
	float scale = 1.0f + 0.1f * sinf(time * 0.005f);


	// 颜色渐变
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
			SDL_FRect rect = {
				centerX - scaledW / 2.0f,
				y,
				scaledW,
				scaledH
			};
			SDL_RenderTexture(renderer, texture, NULL, &rect);
			SDL_DestroyTexture(texture);
		}
		SDL_DestroySurface(surface);
	}
}


//渲染游戏数据
void renderGameOverStats(SDL_Renderer* renderer, TTF_Font* font, GameOverData* data) {
	int centerX = WINDOW_WIDTH / 2;
	int statsY = (WINDOW_HEIGHT - PANEL_HEIGHT) / 2 + 200;
	int lineHeight = 50;
	char buffer[128];

	if (data->isNewHighScore) {
		renderNewHighScoreAnima(renderer, font, centerX,
			statsY - 60, SDL_GetTicks());
	}
	// 渲染分数
	renderTextCentered(renderer, font, "Final Score", centerX, statsY, COLOR_TEXT_SECONDARY);
	snprintf(buffer, sizeof(buffer), "%d", data->finalScore);
	renderTextCentered(renderer, font, buffer, centerX, statsY + 35, COLOR_HIGHLIGHT);

	statsY += lineHeight + 50;

	// 渲染分隔线
	SDL_SetRenderDrawColor(renderer, 80, 90, 100, 255);
	SDL_RenderLine(renderer,
		centerX - 300, statsY,
		centerX + 300, statsY);

	statsY += 30;

	// 详细统计（分两列）：
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

extern void renderButton(SDL_Renderer* renderer, Button* btn, TTF_Font* font);
extern bool isPointInButton(Button* btn, float x, float y);

GameOverAction showGameOverUI(SDL_Renderer* renderer, TTF_Font* titleFont,
	TTF_Font* buttonFont, TTF_Font* smallFont, GameOverData* data) {

	// 创建按钮
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
	GameOverAction action = GAMEOVER_MENU;

	// 渐变效果
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
							//StartSinglePlayer(renderer, smallFont);
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
					action = GAMEOVER_MENU;
					running = false;
				}
			}
		}

		// 渐变
		if (fadingIn) {
			fadeAlpha += 5;
			if (fadeAlpha >= 255) {
				fadeAlpha = 255;
				fadingIn = false;
			}
		}

		//渲染
		renderGameOverBackground(renderer);
		renderGameOverStats(renderer, buttonFont, data);
		drawTitle(renderer, titleFont, "GameOver!!");
		//渲染按钮
		for (int i = 0; i < 3; i++) {
			renderButton(renderer, &buttons[i], buttonFont);
		}
		//提示文字
		renderTextCentered(renderer, smallFont,
			"Press ENTER to restart or ESC to return to menu",
			WINDOW_WIDTH / 2,
			WINDOW_HEIGHT - 50,
			COLOR_TEXT_SECONDARY);

		SDL_RenderPresent(renderer);
		SDL_Delay(16);
	}
	return action;
}