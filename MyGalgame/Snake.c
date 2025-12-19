#include "Snake.h"
#include <stdlib.h>

static Node* newNode(int x, int y) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->x = x; n->y = y; n->next = NULL;
    return n;
}

Snake* Snake_Create(int startX, int startY) {
    Snake* s = (Snake*)malloc(sizeof(Snake));
    s->head = newNode(startX, startY);
    s->dir = DIR_RIGHT;
    s->growPending = false;
    s->head->next = newNode(startX - 1, startY);
    s->head->next->next = newNode(startX - 2, startY);
    return s;
}

void Snake_Destroy(Snake* s) {
    Node* cur = s->head;
    while (cur) { Node* tmp = cur; cur = cur->next; free(tmp); }
    free(s);
}

static void moveHead(Snake* s) {
    int nx = s->head->x, ny = s->head->y;
    switch (s->dir) {
    case DIR_UP:    ny--; break;
    case DIR_DOWN:  ny++; break;
    case DIR_LEFT:  nx--; break;
    case DIR_RIGHT: nx++; break;
    }
    Node* h = newNode(nx, ny);
    h->next = s->head;
    s->head = h;
}

void Snake_Move(Snake* s) {
    moveHead(s);
    if (!s->growPending) {
        Node* cur = s->head;
        while (cur->next->next) cur = cur->next;
        free(cur->next); cur->next = NULL;
    }
    else s->growPending = false;
}

void Snake_Grow(Snake* s) { s->growPending = true; }

bool Snake_SelfCollision(const Snake* s) {
    SDL_Point h = { s->head->x, s->head->y };
    for (Node* cur = s->head->next; cur; cur = cur->next)
        if (cur->x == h.x && cur->y == h.y) return true;
    return false;
}

bool Snake_WallCollision(const Snake* s) {
    int x = s->head->x, y = s->head->y;
    return x < 0 || x >= GRID_W || y < 0 || y >= GRID_H;
}

SDL_Point Snake_HeadGrid(const Snake* s) {
    return (SDL_Point) { s->head->x, s->head->y };
}