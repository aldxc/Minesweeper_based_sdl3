#pragma once

#include <vector>
#include <cstdint>
#include <optional>
#include <random>
#include <utility>

// Board: 负责扫雷棋盘逻辑维护（纯逻辑，无 SDL 依赖）
class Board {
public:
    struct Cell {
        bool isMine{false};    // 是否为地雷
        bool revealed{false};  // 是否已被打开
        bool flagged{false};   // 是否被标记为雷
        uint8_t adjacent{0};   // 相邻地雷数 0-8
        bool dirty{true};      // 脏标记：用于脏格子渲染（默认为脏）
    };

    Board(int rows = 0, int cols = 0, int mines = 0) noexcept;
    ~Board() = default;

    // 重置棋盘尺寸与地雷数
    void resetBoard(int rows, int cols, int mines) noexcept;

    int rows() const noexcept { return rows_; }
    int cols() const noexcept { return cols_; }
    int mineCount() const noexcept { return mineCount_; }

    // 如果需要，在第一次点击后延迟布雷以保证首开安全
    // firstClick: 首次点击位置，可为空
    void ensureMinesPlaced(std::optional<std::pair<int,int>> firstClick = std::nullopt);

    // 打开格子；返回 true 表示成功（非重复打开）
    // 如果踩雷会设置 hitMine_ 为 true
    bool revealCell(int r, int c);

    // 切换标记；如果越界或已打开返回 std::nullopt，否则返回新的标记状态
    std::optional<bool> toggleFlag(int r, int c) noexcept;

    const Cell& at(int r, int c) const noexcept;
    Cell& at(int r, int c) noexcept;

    bool inBounds(int r, int c) const noexcept;
    bool hitMine() const noexcept { return hitMine_; }

    // 未被打开的安全格子数量（用于判定胜利）
    int remainingSafeCells() const noexcept;

    // 脏格子接口：用于渲染层仅重绘改变的格子
    bool isDirty(int r, int c) const noexcept;
    void clearDirty(int r, int c) noexcept;
    void markAllDirty() noexcept;

private:
    void placeMines(int safeR, int safeC);
    void computeAdjacents() noexcept;
    void floodReveal(int r, int c) noexcept;
    int index(int r, int c) const noexcept { return r * cols_ + c; }

    // 成员变量（中文注释）
    std::vector<Cell> cells_;      // 所有格子，按行优先的连续内存
    int rows_{0};                  // 行数
    int cols_{0};                  // 列数
    int mineCount_{0};             // 雷的数量
    bool placed_{false};           // 是否已完成布雷
    bool hitMine_{false};          // 是否已经踩雷

    // 将随机引擎作为成员，符合 RAII 与可控性，便于测试
    std::mt19937 rng_;
};