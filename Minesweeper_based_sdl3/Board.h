#pragma once

#include <vector>
#include <cstdint>
#include <optional>
#include <random>

// Board: 负责扫雷棋盘逻辑维护（纯逻辑，无 SDL 依赖）
class Board {
public:
    struct transInfo {
		int row;
		int col;
		uint8_t state;//0-8数字，9未掀开，10方块，11雷，12旗子
        //可使用位优化
    };
    Board(int rows = 0, int cols = 0, int mines = 0) noexcept;
    ~Board() = default;

    // 重置棋盘尺寸与地雷数
    //rows: 行数，cols: 列数，mines: 雷的数量
    void resetBoard() noexcept;

	void reveal(int r, int c) noexcept; // 打开格子；如果越界或已打开直接返回，否则更新状态并递归打开周围格子
	void doubleClickReveal(int r, int c) noexcept; // 双击打开周围格子；如果越界或未打开直接返回，否则根据标记数量判断是否打开周围格子

    // 切换标记；如果越界或已打开返回 std::nullopt，否则返回新的标记状态
    void toggleFlag(int r, int c) noexcept;

    // 获取需要渲染的格子列表（脏格子）
	const std::vector<transInfo> render() const noexcept; // 获取需要渲染的格子列表（脏格子），并清空脏格子列表
    //const std::vector<uint8_t>& render() const noexcept;
	bool isWin() const noexcept; // 判断是否胜利：所有非雷格子都已打开
	bool isLose() const noexcept { return hitMine_; } // 判断是否失败：是否已经踩雷
	int getRemainingMines() const noexcept { return mineCount_ - flagCount_; } // 获取剩余雷数：总雷数减去已标记的雷数
	int getRows() const noexcept { return rows_; } // 获取行数
	int getCols() const noexcept { return cols_; } // 获取列数
	int getMineCount() const noexcept { return mineCount_; } // 获取雷的数量
	bool isPlaced() const noexcept { return placed_; } // 获取布雷状态

	//位掩码
	static constexpr uint8_t maskIsMine = 1 << 7;//第7位表示是否为雷 1表示是雷，0表示非雷
	static constexpr uint8_t maskRevealed = 1 << 6;//第6位表示是否已被打开 1表示已打开，0表示未打开
	static constexpr uint8_t maskFlagged = 1 << 5;//第5位表示是否被标记为雷 1表示被标记，0表示未标记
	static constexpr uint8_t maskDirty = 1 << 4;//第4位表示是否需要渲染（脏标记）1表示需要渲染，0表示不需要渲染
	static constexpr uint8_t maskNum = 0b00001111;//低四位存相邻地雷数
private:
	void placeMines(int safeR, int safeC);// 在指定安全格周围布雷，确保第一次点击不会踩雷
	void placeNum() noexcept;// 计算每个格子周围的雷数，更新 cells_ 中的相应位
	void floodReveal(int r, int c) noexcept;// 递归打开周围格子，直到遇到非零相邻雷数的格子
    int index(int r, int c) const noexcept { return r * cols_ + c; }
	int countFlagged(int r, int c) const noexcept; // 计算周围被标记为雷的格子数量

    // 成员变量
    int rows_{0};                  // 行数
    int cols_{0};                  // 列数
    int mineCount_{0};             // 雷的数量
	int flagCount_{ 0 };             // 已标记的雷数量，用于显示剩余雷数
    bool placed_{false};           // 是否已完成布雷
    bool hitMine_{false};          // 是否已经踩雷
    std::vector<uint8_t> cells_;      // 所有格子，按行优先的连续内存
	mutable std::vector<transInfo> dirtyCells_; // 需要渲染的格子列表（脏格子）//可变成员，允许在 const 方法中修改，便于记录需要渲染的格子

    // 将随机引擎作为成员，符合 RAII 与可控性，便于测试
    std::mt19937 rng_;
};