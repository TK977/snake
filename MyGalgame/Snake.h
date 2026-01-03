/**
 * snake.h
 * 贪吃蛇数据结构及对外接口
 */

#ifndef SNAKE_H
#define SNAKE_H

#include <SDL3/SDL.h>
#include <stdbool.h>

 /* 网格大小及格子像素尺寸 */
#define GRID_W 60
#define GRID_H 40
#define CELL   20

/* 方向枚举，与键盘输入对应 */
typedef enum { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT } Direction;

/* 链表节点：蛇的每一节 */
typedef struct Node {
    int x, y;
    struct Node* next;
} Node;

/* 蛇对象：头指针 + 当前方向 + 成长标志 */
typedef struct {
    Node* head;
    Direction dir;
    bool growPending; // 下一回合是否成长
} Snake;

/* 对外接口 */
Snake* Snake_Create(int startX, int startY);   // 创建初始 3 节蛇
void   Snake_Destroy(Snake* s);               // 释放整条链表
void   Snake_Move(Snake* s);                  // 走一步（头插+尾删）
void   Snake_Grow(Snake* s);                  // 登记“下一回合要长”
bool   Snake_SelfCollision(const Snake* s);   // 自撞
bool   Snake_WallCollision(const Snake* s);   // 撞墙
SDL_Point Snake_HeadGrid(const Snake* s);     // 获取头节点坐标

#endif // SNAKE_H