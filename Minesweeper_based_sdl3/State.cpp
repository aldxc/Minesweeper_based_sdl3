#include "State.h"
#include "Resource.h"
// State.cpp

//-----------------------------MenuState-----------------------------//
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
	gameNameRect_ = SDL_FRect{ 0, 50, static_cast<float>(w), 100}; // 游戏名称位置和大小，实际使用时需要根据文本尺寸调整
}

void MenuState::renderer() const noexcept{
	Renderer::getInstance().beginRender();

	Renderer::getInstance().renderRects(buttonRects_, SDL_Color(100, 100, 100, 255));
	Renderer::getInstance().renderTexts(buttonLabels_, buttonRects_, SDL_Color(200, 100, 100, 255));
	Renderer::getInstance().renderText("Minesweeper", SDL_Color(200, 100, 100, 255), gameNameRect_, 50);

	Renderer::getInstance().reDefaultAndPresent();
}

std::optional<State::StateTransInfo> MenuState::update(SDL_Event& event) noexcept{
	//handleinput
	if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		SDL_FPoint mousePos{ event.button.x, event.button.y };
		for (int i = 0; i < buttonCount_; ++i) {
			if (SDL_PointInRectFloat(&mousePos, &buttonRects_[i])) {
				return { { StateType::Playing, BORAD_INFO[i][0], BORAD_INFO[i][1], BORAD_INFO[i][2] } }; // 返回状态切换信息，切换到 Playing 状态并传递对应的行列数和地雷数
			}
		}
	}

	return std::nullopt; // 如果 handleInput() 返回 true，表示需要切换到 Playing 状态，否则保持当前状态
}

PlayingState::PlayingState(StateTransInfo info) noexcept : State(StateType::Playing) , board_(info.rows, info.cols, info.mines){

}

//-----------------------------PlayingState-----------------------------//
void PlayingState::renderer() const noexcept{
	Renderer::getInstance().beginRender();



	Renderer::getInstance().reDefaultAndPresent();
}

std::optional<State::StateTransInfo> PlayingState::update(SDL_Event& event) noexcept{

	return std::nullopt;
}

//-----------------------------WonState-----------------------------//
void WonState::renderer() const noexcept{
}


std::optional<State::StateTransInfo> WonState::update(SDL_Event& event) noexcept{
	
	return std::nullopt;
}

//-----------------------------LostState-----------------------------//
void LostState::renderer() const noexcept{
}

std::optional<State::StateTransInfo> LostState::update(SDL_Event& event) noexcept{
	
	return std::nullopt;
}

//-----------------------------PausedState-----------------------------//
void PausedState::renderer() const noexcept{
}

std::optional<State::StateTransInfo> PausedState::update(SDL_Event& event) noexcept{
	
	return std::nullopt;
}
