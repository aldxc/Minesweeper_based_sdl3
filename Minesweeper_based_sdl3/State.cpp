#include "State.h"
// State.cpp

MenuState::MenuState() noexcept : State(StateType::Menu), windowSize_({800, 600}) {
	// 初始化按钮位置和大小
	float buttonWidth = 200.0;
	float buttonHeight = 50.0;
	auto [w, h] = windowSize_;
	for(int i = 0; i < buttonCount_; ++i) {
		float x = (w - buttonWidth) / 2.0f;
		float y = (h - buttonHeight * buttonCount_) / 2.0f + i * (buttonHeight + 20);
		buttonRects_.emplace_back(SDL_FRect{ x, y, buttonWidth, buttonHeight });
	}
	buttonLabels_ = { "Easy", "Medium", "Hard" };
}

void MenuState::renderer() const noexcept{
	Renderer::getInstance().beginRender();
	Renderer::getInstance().renderRects(buttonRects_, SDL_Color(100, 100, 100, 255));
	Renderer::getInstance().renderTexts(buttonLabels_, buttonRects_, SDL_Color(200, 100, 100, 255));
	Renderer::getInstance().reDefaultAndPresent();
}

bool MenuState::handleInput() noexcept{

	return false;
}

std::optional<StateType> MenuState::update() noexcept{
	MenuState::renderer();
	bool flag = MenuState::handleInput();

	return flag ? std::optional<StateType>(StateType::Playing) : std::nullopt; // 如果 handleInput() 返回 true，表示需要切换到 Playing 状态，否则保持当前状态
}

void PlayingState::renderer() const noexcept{
}

bool PlayingState::handleInput() noexcept{
	return false;
}

std::optional<StateType> PlayingState::update() noexcept{
	PlayingState::renderer();
	PlayingState::handleInput();

	return std::nullopt;
}

void WonState::renderer() const noexcept{
}

bool WonState::handleInput() noexcept{

	return false;

}

std::optional<StateType> WonState::update() noexcept{
	WonState::renderer();
	WonState::handleInput();
	return std::nullopt;
}

void LostState::renderer() const noexcept{
}

bool LostState::handleInput() noexcept{
	return false;

}

std::optional<StateType> LostState::update() noexcept{
	LostState::renderer();
	LostState::handleInput();
	return std::nullopt;
}

void PausedState::renderer() const noexcept{
}

bool PausedState::handleInput() noexcept{
	return false;

}

std::optional<StateType> PausedState::update() noexcept{
	PausedState::renderer();
	PausedState::handleInput();
	return std::nullopt;
}
