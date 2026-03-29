#include "Game.h"

// 实现 Game 类方法
Game::Game() : stateMachine_(StateMachine()), running_(true) {
	// 初始化 SDL 视频和事件子系统
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return;
	}
	// 初始化 SDL_ttf 库，必须在创建窗口和渲染器之前调用
	if (!TTF_Init()) {
		SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
		return;
	}
}

void Game::Run() noexcept {

	while (running_) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if(event.type == SDL_EVENT_QUIT) {
				running_ = false; // 处理退出事件，设置运行标志为 false 以退出主循环
			}

			stateMachine_.update(event);
		}
		stateMachine_.render(); // 渲染当前状态

		SDL_Delay(16);//
	}
}