#pragma once

#include <vector>
#include <cstdint>
#include <optional>
#include <random>

// Board: 负责扫雷棋盘逻辑维护（纯逻辑，无 SDL 依赖）
class Board {
public:
    Board(int rows = 0, int cols = 0, int mines = 0) noexcept;
    ~Board() = default;

    // 重置棋盘尺寸与地雷数
    //rows: 行数，cols: 列数，mines: 雷的数量
    void resetBoard(int rows, int cols, int mines) noexcept;

    // 切换标记；如果越界或已打开返回 std::nullopt，否则返回新的标记状态
    std::optional<bool> toggleFlag(int r, int c) noexcept;

    //位掩码
	static constexpr uint8_t maskIsMine = 1 << 8;//第8位表示是否为雷
	static constexpr uint8_t maskRevealed = 1 << 7;//第7位表示是否已被打开
	static constexpr uint8_t maskFlagged = 1 << 6;//第6位表示是否被标记为雷
	static constexpr uint8_t maskDirty = 1 << 5;//第5位表示是否需要渲染（脏标记）
	static constexpr uint8_t maskAdjacent = 0b00001111;//低四位存相邻地雷数
private:
	void placeMines(int safeR, int safeC);// 在指定安全格周围布雷，确保第一次点击不会踩雷
	void computeAdjacents() noexcept;// 计算每个格子周围的雷数，更新 cells_ 中的相应位
	void floodReveal(int r, int c) noexcept;// 递归打开周围格子，直到遇到非零相邻雷数的格子
    int index(int r, int c) const noexcept { return r * cols_ + c; }

    // 成员变量
    int rows_{0};                  // 行数
    int cols_{0};                  // 列数
    int mineCount_{0};             // 雷的数量
    //bool placed_{false};           // 是否已完成布雷
    bool hitMine_{false};          // 是否已经踩雷
    std::vector<uint8_t> cells_;      // 所有格子，按行优先的连续内存


    // 将随机引擎作为成员，符合 RAII 与可控性，便于测试
    std::mt19937 rng_;
};