#include "Board.h"
#include <algorithm>
// 实现 Board 类方法

Board::Board(int rows, int cols, int mines) noexcept : rows_(rows), cols_(cols), mineCount_(mines), hitMine_(false), cells_(std::vector<uint8_t>(rows_ * cols_)) {
	
}

void Board::resetBoard(int rows, int cols, int mines) noexcept{
	//placeMines();
}

void Board::placeMines(int safeR, int safeC){

	// 构建候选位置列表，排除禁止区（3x3）
	std::vector<int> candidates;
	candidates.reserve(rows_ * cols_);
	for (int pos = 0; pos < mineCount_; ++pos) {
		int r = pos / this->cols_;
		int c = pos % this->cols_;
		bool in_forbid = false;
		if (safeR >= 0 && safeC >= 0) {
			if (abs(r - safeR) <= 1 && abs(c - safeC) <= 1) in_forbid = true;
		}
		if (!in_forbid) candidates.push_back(pos);
	}

	// 防御：若候选位置小于雷数，降为候选大小（极端小地图情况下）
	if (static_cast<int>(candidates.size()) < mineCount_) 
		mineCount_ = static_cast<int>(candidates.size());

	std::shuffle(candidates.begin(), candidates.end(), rng_); // 随机打乱候选位置
}
