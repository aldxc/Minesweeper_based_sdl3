#include "State.h"
#include "Resource.h"
// State.cpp

//-----------------------------MenuState-----------------------------//
MenuState::MenuState() noexcept : State(StateType::Menu), windowSize_({800, 600}) {
	// 初始化按钮位置和大小
	float buttonWidth = 200.0;
	float buttonHeight = 50.0;
	auto [w, h] = windowSize_;
	Renderer::getInstance().updateWindow(w, h); // 调整窗口大小，更新相关资源
	for(int i = 0; i < buttonCount_; ++i) {
		float x = (w - buttonWidth) / 2.0f;
		float y = (h - buttonHeight * buttonCount_) / 2.0f + i * (buttonHeight + 20);
		buttonRects_.emplace_back(SDL_FRect{ x, y, buttonWidth, buttonHeight });
	}
	buttonLabels_ = { "Easy", "Medium", "Hard" };
	gameNameRect_ = SDL_FRect{ 0, 50, static_cast<float>(w), 100}; // 游戏名称位置和大小，实际使用时需要根据文本尺寸调整
}

void MenuState::renderer(bool flag) const noexcept{
	Renderer::getInstance().renderRects(buttonRects_, SDL_Color(100, 100, 100, 255));
	Renderer::getInstance().renderTexts(buttonLabels_, buttonRects_, SDL_Color(200, 100, 100, 255));
	Renderer::getInstance().renderText("Minesweeper", SDL_Color(200, 100, 100, 255), gameNameRect_, 50);
}

std::optional<State::StateTransInfo> MenuState::update(SDL_Event& event) noexcept{
	//handleinput
	if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		SDL_FPoint mousePos{ event.button.x, event.button.y };
		for (int i = 0; i < buttonCount_; ++i) {
			if (SDL_PointInRectFloat(&mousePos, &buttonRects_[i])) {
				return { { StateType::Playing, BOARD_INFO[i][0], BOARD_INFO[i][1], BOARD_INFO[i][2] } }; // 返回状态切换信息，切换到 Playing 状态并传递对应的行列数和地雷数
			}
		}
	}

	return std::nullopt; // 如果 handleInput() 返回 true，表示需要切换到 Playing 状态，否则保持当前状态
}

//-----------------------------PlayingState-----------------------------//
PlayingState::PlayingState(StateTransInfo info) noexcept : State(StateType::Playing) , board_(info.rows, info.cols, info.mines){
	Renderer::getInstance().updateWindow(info.cols * CELL_SIZE, (info.rows + UP_BLOCKS) * CELL_SIZE); // 调整窗口大小以适应棋盘
	upBlocks_ = {
		SDL_FRect{static_cast<float>((info.cols * CELL_SIZE - 2 * CELL_SIZE) / 2), 0, CELL_SIZE * 2, CELL_SIZE * 2},//restart
		SDL_FRect{0, 0, CELL_SIZE * 2, CELL_SIZE * 2},//timer
		SDL_FRect{static_cast<float>(info.cols * CELL_SIZE - 2 * CELL_SIZE), 0, CELL_SIZE * 2, CELL_SIZE * 2},//mine count
		SDL_FRect{static_cast<float>(info.cols * CELL_SIZE - 2 * CELL_SIZE), CELL_SIZE * 2, CELL_SIZE * 2, CELL_SIZE * 2}//pause
	};

	timer_ = 0.0f;
	lastTime_ = std::chrono::high_resolution_clock::now();

	//test
	tempWin = { 0,0,CELL_SIZE, CELL_SIZE };
}

void PlayingState::renderer(bool flag) const noexcept{
	updateTimer(flag); // 更新计时器

	auto& d = board_.render();
	Renderer::getInstance().renderDirtyBlocks(d); // 渲染脏格子，仅重绘改变的格子，优化性能
	//render UI
	Renderer::getInstance().renderRects(upBlocks_, SDL_Color(100, 100, 100, 255)); // 绘制上方功能区背景
	std::vector<std::string> upBlockLabels = { "Restart", std::to_string(static_cast<int>(timer_)), std::to_string(board_.getRemainingMines()), "Pause"};
	Renderer::getInstance().renderTexts(upBlockLabels, upBlocks_, SDL_Color(100, 100, 100, 255));

	//test
	Renderer::getInstance().renderRect(tempWin, SDL_Color(255, 0, 0, 255)); 

}

std::optional<State::StateTransInfo> PlayingState::update(SDL_Event& event) noexcept{
	int col = event.button.x / CELL_SIZE;
	if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		int row = (event.button.y / CELL_SIZE) - UP_BLOCKS;

		//SDL_Log("Mouse click at row %d, col %d", row, col);
		if (col >= 0 && row >= 0) {
			if (event.button.button == SDL_BUTTON_LEFT) {
				if (event.button.clicks >= 1) {
					board_.doubleClickReveal(row, col); // 双击打开周围格子
				}
				board_.reveal(row, col); // 打开格子
			} else if (event.button.button == SDL_BUTTON_RIGHT) {
				board_.toggleFlag(row, col); // 切换标记
			}
		}
		if (event.button.y >= 0 && event.button.x >= 0) {
			SDL_FPoint mousePos{ event.button.x, event.button.y };
			if (SDL_PointInRectFloat(&mousePos, &upBlocks_[0])) {
				board_.resetBoard(); // 重置棋盘
				timer_ = 0.0f;
				lastTime_ = std::chrono::high_resolution_clock::now();
			}else if (SDL_PointInRectFloat(&mousePos, &upBlocks_[3])) {
				return { { StateType::Paused, board_.getRows(), board_.getCols(), board_.getMineCount() } };
			}

			//test
			if (SDL_PointInRectFloat(&mousePos, &tempWin)) {
				return { { StateType::Won, board_.getRows(), board_.getCols(), board_.getMineCount()} };
			}
		}
	}

	if(board_.isWin()) {
		std::vector<std::string> ui = { "Restart", "00:00", "0", "Pause"};
		Renderer::getInstance().saveBoardUI(upBlocks_, ui, 30, SDL_Color(100, 100, 100, 255));
		return { { StateType::Won, board_.getRows(), board_.getCols(), board_.getMineCount()}}; // 切换到 Won 状态
	} else if (board_.isLose()) {
		return { { StateType::Lost, board_.getRows(), board_.getCols(), board_.getMineCount()} }; // 切换到 Lost 状态
	}

	return std::nullopt;
}

void PlayingState::updateTimer(bool flag) const noexcept{
	// 更新计时器
	double deltaTime = 0.0;
	if (board_.isPlaced()) { // 只有在布雷完成后才开始计时
		auto now = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float>(now - lastTime_).count();
		lastTime_ = now;
		if (!flag) deltaTime = 0.0;
	}
	timer_ += deltaTime;
}

//-----------------------------EndState-----------------------------//
EndState::EndState(StateTransInfo info) noexcept : State(StateType::Won),info_(info){
	int windowWidth = 0, windowHeight = 0;
	SDL_GetWindowSize(Renderer::getInstance().getSDLWindow(), &windowWidth, &windowHeight);

	winMessageRect_ = { static_cast<float>(windowWidth / 4), UP_BLOCKS * CELL_SIZE / 2, static_cast<float>(windowWidth / 2) , static_cast<float>(windowHeight / 3) };
	restartButtonRect_ = { static_cast<float>(windowWidth * 3 / 8) , winMessageRect_.y + winMessageRect_.h , static_cast<float>(windowWidth / 4), static_cast<float>(windowHeight / 8) };
	backToMenuButtonRect_ = { static_cast<float>(windowWidth * 3 / 8) , restartButtonRect_.y + restartButtonRect_.h, static_cast<float>(windowWidth / 4), static_cast<float>(windowHeight / 8) };
}

void EndState::renderer(bool flag) const noexcept{
	//render UI
	Renderer::getInstance().renderBoardUI(); // 渲染保存的棋盘 UI 纹理，供结束后 win/lose 界面使用，避免重复渲染棋盘
	Renderer::getInstance().renderDirtyBlocks(); // 渲染脏格子，实际使用时需要根据 Board 类的接口调整

	Renderer::getInstance().renderRect(winMessageRect_, SDL_Color(200, 100, 100, 255)); // 
	Renderer::getInstance().renderRect(restartButtonRect_, SDL_Color(200, 100, 100, 255)); // 绘制重开按钮背景
	Renderer::getInstance().renderRect(backToMenuButtonRect_, SDL_Color(200, 100, 100, 255)); // 绘制返回菜单按钮背景
	std::string str = (info_.state == StateType::Won) ? "You Win !" : "You Lose !";
	Renderer::getInstance().renderText(str, SDL_Color(200, 100, 100, 255), winMessageRect_, 50);
	Renderer::getInstance().renderText("restart", SDL_Color(0, 0, 0, 255), restartButtonRect_, 30);
	Renderer::getInstance().renderText("back to menu", SDL_Color(0, 0, 0, 255), backToMenuButtonRect_, 30);
}


std::optional<State::StateTransInfo> EndState::update(SDL_Event& event) noexcept{
	if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		if (event.button.y >= 0 && event.button.x >= 0) {
			SDL_FPoint mousePos{ event.button.x, event.button.y };
			if (SDL_PointInRectFloat(&mousePos, &restartButtonRect_)) {
				info_.state = StateType::Playing; // 切换到 Playing 状态
				return info_;
			}
			else if (SDL_PointInRectFloat(&mousePos, &backToMenuButtonRect_)) {
				return { { StateType::Menu, 0, 0, 0 } };
			}
		}
	}
	return std::nullopt;
}

//---------------------------PausedState-----------------------------//
PausedState::PausedState(StateTransInfo info) noexcept : State(StateType::Paused), info_(info) {
	int windowWidth = 0, windowHeight = 0;
	SDL_GetWindowSize(Renderer::getInstance().getSDLWindow(), &windowWidth, &windowHeight);

	buttonRects_.resize(3);
	buttonRects_[0] = SDL_FRect{ static_cast<float>(windowWidth * 3 / 8) ,  UP_BLOCKS * CELL_SIZE / 2 , static_cast<float>(windowWidth / 4), static_cast<float>(windowHeight / 8) }; // resume
	
	for (int i = 1; i < 3; ++i) {
		buttonRects_[i] = SDL_FRect{
			buttonRects_[i - 1].x,
			buttonRects_[i - 1].y + buttonRects_[i - 1].h,
			buttonRects_[i - 1].w,
			buttonRects_[i - 1].h
		};
	}
	//resume //restart //back to menu
}

void PausedState::renderer(bool flag) const noexcept{
	Renderer::getInstance().renderRects(buttonRects_, SDL_Color(100, 100, 100, 255)); // 绘制按钮背景
	Renderer::getInstance().renderTexts({ "Resume", "Restart", "Back to Menu" }, buttonRects_, SDL_Color(255, 255, 255, 255)); // 绘制按钮文本
}

std::optional<State::StateTransInfo> PausedState::update(SDL_Event& event) noexcept{
	if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		if (event.button.y >= 0 && event.button.x >= 0) {
			SDL_FPoint mousePos{ event.button.x, event.button.y };
			if (SDL_PointInRectFloat(&mousePos, &buttonRects_[0])) {
				return { { StateType::Playing, -1, -1, -1 } }; // 切换回 Playing 状态
			}
			else if (SDL_PointInRectFloat(&mousePos, &buttonRects_[1])) {
				info_.state = StateType::Playing; // 切换到 Playing 状态
				return info_; // 切换到 Playing 状态，实际使用时需要传递对应的行列数和地雷数
			}
			else if (SDL_PointInRectFloat(&mousePos, &buttonRects_[2])) {
				return { { StateType::Menu, 0, 0, 0 } }; // 切换到 Menu 状态
			}
		}
	}

	return std::nullopt;
}
