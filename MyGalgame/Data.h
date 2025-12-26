#pragma once
#include "GameTypes.h" // for RankingEntry

// 加载最多 max_entries 条排行榜记录。返回实际加载的记录数量。
// 如果文件不存在，返回 0 或采用默认条目，取决于实现。

int LoadRankings(RankingEntry* entries, int max_entries);

// 保存一条新的分数记录。
// 函数会加载现有记录，加入新记录，排序后将前10名写回到文件。
void SaveRanking(const char* name, int score);

// 更新排行榜中所有匹配的玩家姓名
void UpdatePlayerName(const char* oldName, const char* newName);
