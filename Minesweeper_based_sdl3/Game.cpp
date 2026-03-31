#include "Game.h"

// 实现 Game 类方法
Game::Game() : running_(true) {
	if (!init()) {
		SDL_Log("Failed to initialize game."); // 初始化失败
		return;
	}
	stateMachine_ = std::make_unique<StateMachine>(); // 创建状态机实例，初始状态为菜单

	resource_ = std::make_unique<Resource>();
}

void Game::Run() noexcept {

	while (running_) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if(event.type == SDL_EVENT_QUIT) {
				running_ = false; 
			}

			stateMachine_->update(event);
		}
		stateMachine_->render(); // 渲染当前状态

		SDL_Delay(16);//控制帧率为约 60 FPS
	}
}

bool Game::init() noexcept{
	// 初始化 SDL 视频和事件子系统
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return false;
	}
	// 初始化 SDL_ttf 库，必须在创建窗口和渲染器之前调用
	if (!TTF_Init()) {
		SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
		return false;
	}
	return true;
}


