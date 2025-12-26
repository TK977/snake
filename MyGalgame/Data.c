#define _CRT_SECURE_NO_WARNINGS
#include "Data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h> // _mkdir 在这个库里面
#include <stdbool.h>

#define DATA_DIR "data"
#define DATA_FILE "data/rankings.dat"

// 确保数据目录存在
static void EnsureDataDir() {
    _mkdir(DATA_DIR);
}


// 比较函数用于排序排行榜（按分数降序）
static int CompareEntries(const void* a, const void* b) {
    const RankingEntry* entryA = (const RankingEntry*)a;
    const RankingEntry* entryB = (const RankingEntry*)b;
    return entryB->score - entryA->score; 
}

int LoadRankings(RankingEntry* rankings, int max_entries) {
    EnsureDataDir();
    FILE* file = fopen(DATA_FILE, "r");
    
    // 如果文件不存在，返回0表示没有数据。
    if (file == NULL) {
        return 0; 
    }

    int count = 0;
    while (count < max_entries && fscanf(file, "%d %s %d",
        &rankings[count].rank,
        rankings[count].name,
        &rankings[count].score) == 3) {
        count++;
    }
    fclose(file);
    return count;
}

void SaveRanking(const char* name, int score) {
    EnsureDataDir();
    
    // 加载现有排名
    RankingEntry entries[100]; // 使用大于10的数防止溢出
    int count = LoadRankings(entries, 99);
    
    // 添加新条目
    // 如果用户没有设置名称，使用默认名 "Player"
    if (name == NULL || strlen(name) == 0) {
        name = "Player";
    }

    strncpy(entries[count].name, name, 19);
    entries[count].name[19] = '\0';
    entries[count].score = score;
    entries[count].rank = 0; 
    count++;

    // 排序（按分数降序）
    qsort(entries, count, sizeof(RankingEntry), CompareEntries);

    // 添加新数据之后只保留前10名并写回文件
    FILE* file = fopen(DATA_FILE, "w");
    if (!file) return;

    int write_count = count > 10 ? 10 : count;
    for (int i = 0; i < write_count; i++) {
        entries[i].rank = i + 1;
        fprintf(file, "%d %s %d\n", entries[i].rank, entries[i].name, entries[i].score);
    }

    fclose(file);
}


// 检查并更新玩家名称（把所有匹配的旧名替换为新名）
void UpdatePlayerName(const char* oldName, const char* newName) {
    // 如果旧名称和新名称相同或为空，则不进行任何操作
    if (!oldName || !newName || strcmp(oldName, newName) == 0) return;

    EnsureDataDir();
    RankingEntry entries[100];
    int count = LoadRankings(entries, 100);
    bool changed = false;

    for (int i = 0; i < count; i++) {
        if (strcmp(entries[i].name, oldName) == 0) {
            strncpy(entries[i].name, newName, 19);
            entries[i].name[19] = '\0';
            changed = true;
        }
    }

    if (changed) {
        FILE* file = fopen(DATA_FILE, "w");
        if (file) {
            for (int i = 0; i < count; i++) {
                fprintf(file, "%d %s %d\n", entries[i].rank, entries[i].name, entries[i].score);
            }
            fclose(file);
        }
    }
}
