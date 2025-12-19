#pragma once
typedef struct {
	char name[20];
	float score;
	int rank;
}RankingEntry;


void renderRankingTitle(SDL_Renderer* renderer, TTF_Font* font, const char* text, float y);

void renderRankingEntry(SDL_Renderer* renderer, TTF_Font* font, RankingEntry* entry, float y, bool isHovered);

void RenderHead(SDL_Renderer* renderer, TTF_Font* font, float y);

int LoadData(RankingEntry* Rankings, int MaxEntries);

void LookRankings(SDL_Renderer* renderer, TTF_Font* TitleFont, TTF_Font* NormallFont, TTF_Font* SmallFont);