#include "Game.h"

// 实现 Game 类方法
Game::Game() : stateMachine_(StateMachine()), running_(true) {
	//state默认menu,二次调用确保状态为menu
	stateMachine_.changeStates(StateType::Menu);
}

void Game::Run() noexcept {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS); // 初始化 SDL 视频和事件子系统

	while (running_) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if(event.type == SDL_EVENT_QUIT) {
				running_ = false; // 处理退出事件，设置运行标志为 false 以退出主循环
			}
		}
		stateMachine_.update();
	}
}