#pragma once
#include "StateMachine.h"
#include <memory>
#include <SDL3/SDL.h>

class Game {
public:
	Game();
	~Game() = default;
	void Run() noexcept; // 游戏主循环，负责调用状态机的 update() 方法
private:
	StateMachine stateMachine_;
	bool running_{ true }; // 游戏运行标志，控制主循环退出
};
