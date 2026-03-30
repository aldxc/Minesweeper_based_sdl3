#include "Board.h"
#include <algorithm>
// 实现 Board 类方法

Board::Board(int rows, int cols, int mines) noexcept : rows_(rows), cols_(cols), mineCount_(mines), hitMine_(false), placed_(false),rng_(std::random_device()()) {

	resetBoard(); // 初始化重置棋盘状态
}

void Board::resetBoard() noexcept{
	cells_.clear(); // 清空原有格子状态
	cells_.resize(rows_ * cols_); // 预留足够空间，避免频繁 realloc
	dirtyCells_.reserve(rows_ * cols_); // 预留足够空间，避免频繁 realloc
	for (int i = 0; i < rows_ * cols_; ++i) {
		cells_[i] = 0b00010000; // 初始时所有格子都未掀开（第7位为0），不含雷（第8位为0），未标记（第6位为0），需要渲染（第5位为1）
		dirtyCells_.push_back({ i / cols_, i % cols_, 9 }); // 初始时所有格子状态为9（未掀开）
	}
	hitMine_ = false; // 重置踩雷状态
	placed_ = false; // 重置布雷状态，等待首次点击时布雷
	flagCount_ = 0; // 重置标记数量
}

void Board::reveal(int r, int c) noexcept{
	if (!placed_) {
		placeMines(r, c); // 首次点击时布雷，确保安全区
		placeNum(); // 计算数字
		placed_ = true;
	}
	//if (cells_[index(r, c)] & maskRevealed) return; // 已经打开，直接返回
	//后续需要自动打开周围格子时，不能因为已经打开而阻止继续递归，因此不在此处直接返回，而是让后续逻辑处理
	if (cells_[index(r, c)] & maskFlagged) return; // 已经标记为雷，不能打开，直接返回)
	if (cells_[index(r, c)] & maskIsMine) {
		dirtyCells_.push_back({ r, c, 11 }); // 踩雷状态
		hitMine_ = true;
		return;
	}
	
	uint8_t s = cells_[index(r, c)] & maskNum;
	floodReveal(r, c);
}

void Board::doubleClickReveal(int r, int c) noexcept{
	if(cells_[index(r, c)] & maskRevealed) { // 只有已打开的格子才响应双击
		int flaggedCount = countFlagged(r, c);
		uint8_t num = cells_[index(r, c)] & maskNum;
		if (flaggedCount == num) { // 如果标记数量等于数字，打开周围未标记的格子
			for (int dr = -1; dr <= 1; ++dr) {
				for (int dc = -1; dc <= 1; ++dc) {
					if (dr == 0 && dc == 0) continue;
					int nr = r + dr;
					int nc = c + dc;
					if (nr >= 0 && nr < rows_ && nc >= 0 && nc < cols_) {
						if (!(cells_[index(nr, nc)] & maskFlagged)) {
							reveal(nr, nc); // 打开周围未标记的格子
						}
					}
				}
			}
		}
	}
}

void Board::toggleFlag(int r, int c) noexcept{
	if (!(cells_[index(r, c)] & maskRevealed)) {
		cells_[index(r, c)] ^= maskFlagged; // 切换标记状态
		flagCount_ += (cells_[index(r, c)] & maskFlagged) ? 1 : -1; // 更新标记数量
		uint8_t s = cells_[index(r, c)] & maskFlagged ? 12 : 9; // 更新状态：12表示旗子，9表示未掀开
		dirtyCells_.push_back({ r, c, s }); // 添加到脏格子列表
	}
}

const std::vector<Board::transInfo> Board::render() const noexcept{
	std::vector<transInfo> toRender = std::move(dirtyCells_); // 获取当前脏格子列表
	dirtyCells_.clear(); // 清空脏格子列表，准备下一轮更新
	return toRender; // 返回需要渲染的格子列表
}

bool Board::isWin() const noexcept{
	for (int i = 0; i < rows_ * cols_; ++i) {
		if (!(cells_[i] & maskIsMine) && !(cells_[i] & maskRevealed)) {
			return false; // 只要有一个非雷格子未被打开，就不是胜利
		}
	}
	return true;
}

//const std::vector<uint8_t>& Board::render() const noexcept {
//	return cells_; // 返回当前所有格子的状态，实际使用时需要根据 Board 类的接口调整
//}


void Board::placeMines(int safeR, int safeC){

	// 构建候选位置列表，排除禁止区（3x3）
	std::vector<int> candidates;
	candidates.reserve(rows_ * cols_);
	for (int pos = 0; pos < rows_ * cols_; ++pos) {
		int r = pos / cols_;
		int c = pos % cols_;
		if (safeR >= 0 && safeC >= 0) {
			if (std::abs(r - safeR) <= 1 && std::abs(c - safeC) <= 1) continue;
		}
		candidates.push_back(pos);
	}

	// 防御：若候选位置小于雷数，降为候选大小（极端小地图情况下）
	if (static_cast<int>(candidates.size()) < mineCount_) 
		mineCount_ = static_cast<int>(candidates.size());

	std::shuffle(candidates.begin(), candidates.end(), rng_); // 随机打乱候选位置

	//cells_.clear(); // 清空原有格子状态
	//cells_.resize(rows_ * cols_);
	std::fill(cells_.begin(), cells_.end(), 0);// 重置所有格子状态为“需要渲染”（第5位为1），其他位为0

	for (int i = 0; i < mineCount_; ++i) {
		int pos = candidates[i];
		cells_[pos] |= maskIsMine; // 设置雷标记
	}
}

void Board::placeNum() noexcept{
	for (int i = 0; i < rows_ * cols_; ++i) {
		if(!(cells_[i] & maskIsMine)) {
			int r = i / cols_;
			int c = i % cols_;
			for (int dr = -1; dr <= 1; ++dr) {
				for (int dc = -1; dc <= 1; ++dc) {
					if (dr == 0 && dc == 0) continue;
					int nr = r + dr;
					int nc = c + dc;
					if (nr >= 0 && nr < rows_ && nc >= 0 && nc < cols_) {
						cells_[i] += (cells_[index(nr, nc)] & maskIsMine) ? 1 : 0; // 累加相邻雷数
					}
				}
			}
		}
	}
}

void Board::floodReveal(int r, int c) noexcept{
	if (r < 0 || r >= rows_ || c < 0 || c >= cols_) return; // 越界检查
	int idx = index(r, c);
	if (cells_[idx] & maskRevealed) return; // 已经打开，直接返回
	cells_[idx] |= maskRevealed; // 设置已打开标记

	if (cells_[idx] & maskIsMine) {
		dirtyCells_.push_back({ r, c, 11 }); // 踩雷状态
		hitMine_ = true;
		return;
	}

	uint8_t s = cells_[idx] & maskNum; // 获取数字状态
	dirtyCells_.push_back(transInfo{ r, c, s }); // 添加到脏格子列表
	if (s == 0) { // 如果是空格，继续递归打开周围格子
		for (int dr = -1; dr <= 1; ++dr) {
			for (int dc = -1; dc <= 1; ++dc) {
				if (dr == 0 && dc == 0) continue;
				floodReveal(r + dr, c + dc);
			}
		}
	}
}

int Board::countFlagged(int r, int c) const noexcept{
	if (cells_[index(r, c)] & maskIsMine) return 0; // 雷格子不计算标记数量
	int count = 0;
	for (int dr = -1; dr <= 1; ++dr) {
		for (int dc = -1; dc <= 1; ++dc) {
			if (dr == 0 && dc == 0) continue;
			int nr = r + dr;
			int nc = c + dc;
			if (nr >= 0 && nr < rows_ && nc >= 0 && nc < cols_) {
				if (cells_[index(nr, nc)] & maskFlagged) {
					++count;
				}
			}
		}
	}
	return count;
}
