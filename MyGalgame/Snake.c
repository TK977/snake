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
    Node* n = (Node*)malloc(sizeof(Node));// 申请内存
    n->x = x; 
    n->y = y;                              // 存坐标
    n->next = NULL;                 // 后面还没接东西
}

/**
 * 创建一条初始 3 节的蛇，水平向右
 * @param startX 头节点 x
 * @param startY 头节点 y
 * @return 蛇对象指针
 */
Snake* Snake_Create(int startX, int startY)
{
    Snake* s = (Snake*)malloc(sizeof(Snake));    // 蛇对象本身
    s->head = newNode(startX, startY);  // 第一节=蛇头
    s->dir = DIR_RIGHT;             // 默认向右
    s->growPending = false; // 没吃到食物
    // 再串两节到左边，形成“头→身→尾”
    s->head->next = newNode(startX - 1, startY);
    s->head->next->next = newNode(startX - 2, startY);
    return s;
}

/**
 * 释放整条链表 + 蛇对象
 */
void Snake_Destroy(Snake* s)
{
    Node* cur = s->head;    // 从头删到尾
    while (cur) {
        Node* tmp = cur;
        cur = cur->next;
        free(tmp);// 释放一节
    }
    free(s);// 最后释放蛇对象本身
}

/**
 * 只移动头节点（内部函数）
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
    Node* h = newNode(nx, ny);// 新建节点=新头
    h->next = s->head;// 旧头变成第二节
    s->head = h;// 更新头指针
}

/**
 * 蛇向前移动一格：
 * 1. 头插新节点
 * 2. 若无成长请求，删尾节；否则保留（实现成长）
 */
void Snake_Move(Snake* s)
{
    moveHead(s);        // 1. 先把头往前挪
    if (!s->growPending) {  // 2. 如果“没吃到食物”
        /* 找到倒数第二节 */
        Node* cur = s->head;
        while (cur->next->next) cur = cur->next;// 找到倒数第二节
        free(cur->next);        // 3. 删尾节
        cur->next = NULL;
    }
    else {
        s->growPending = false; // 吃到食物就保留尾节（完成成长）
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