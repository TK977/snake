/**
 * snake.c
 * 贪吃蛇数据结构：创建、销毁、移动、成长、碰撞检测
 */

#include "Snake.h"
#include <stdlib.h>

 /**
  * 新建一个节点（身体节）
  */
static Node* newNode(int x, int y)
{
    Node* n = (Node*)malloc(sizeof(Node));
    n->x = x; n->y = y; n->next = NULL;
    return n;
}

/**
 * 创建一条初始 3 节的蛇，水平向右
 * @param startX 头节点 x
 * @param startY 头节点 y
 * @return 蛇对象指针
 */
Snake* Snake_Create(int startX, int startY)
{
    Snake* s = (Snake*)malloc(sizeof(Snake));
    s->head = newNode(startX, startY);
    s->dir = DIR_RIGHT;
    s->growPending = false; // 暂无成长请求
    /* 第二节、第三节向左延伸 */
    s->head->next = newNode(startX - 1, startY);
    s->head->next->next = newNode(startX - 2, startY);
    return s;
}

/**
 * 释放整条链表 + 蛇对象
 */
void Snake_Destroy(Snake* s)
{
    Node* cur = s->head;
    while (cur) {
        Node* tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    free(s);
}

/**
 * 移动头节点（内部函数）
 */
static void moveHead(Snake* s)
{
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

/**
 * 蛇向前移动一格：
 * 1. 头插新节点
 * 2. 若无成长请求，删尾节；否则保留（实现成长）
 */
void Snake_Move(Snake* s)
{
    moveHead(s);
    if (!s->growPending) {
        /* 找到倒数第二节 */
        Node* cur = s->head;
        while (cur->next->next) cur = cur->next;
        free(cur->next);
        cur->next = NULL;
    }
    else {
        s->growPending = false; // 完成成长
    }
}

/**
 * 请求下一回合成长（吃到食物时调用）
 */
void Snake_Grow(Snake* s)
{
    s->growPending = true;
}

/**
 * 自撞检测：头节点坐标与任意身体节重合
 */
bool Snake_SelfCollision(const Snake* s)
{
    SDL_Point h = { s->head->x, s->head->y };
    for (Node* cur = s->head->next; cur; cur = cur->next)
        if (cur->x == h.x && cur->y == h.y) return true;
    return false;
}

/**
 * 撞墙检测：头节点超出网格
 */
bool Snake_WallCollision(const Snake* s)
{
    int x = s->head->x, y = s->head->y;
    return x < 0 || x >= GRID_W || y < 0 || y >= GRID_H;
}

/**
 * 获取头节点网格坐标（供外部碰撞、吃食物用）
 */
SDL_Point Snake_HeadGrid(const Snake* s)
{
    return (SDL_Point) { s->head->x, s->head->y };
}