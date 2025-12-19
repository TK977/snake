#ifndef SNAKE_H
#define SNAKE_H

#include <SDL3/SDL.h>
#include <stdbool.h>

#define GRID_W 60
#define GRID_H 40
#define CELL   20

typedef enum { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT } Direction;

typedef struct Node {
    int x, y;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Direction dir;
    bool growPending;
} Snake;

Snake* Snake_Create(int startX, int startY);
void   Snake_Destroy(Snake* s);
void   Snake_Move(Snake* s);
void   Snake_Grow(Snake* s);
bool   Snake_SelfCollision(const Snake* s);
bool   Snake_WallCollision(const Snake* s);
SDL_Point Snake_HeadGrid(const Snake* s);

#endif