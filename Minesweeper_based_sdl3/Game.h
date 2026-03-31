#pragma once
#include <memory>
#include <SDL3/SDL.h>
#include "StateMachine.h"
#include "Resource.h"

class Game {
public:
	Game();
	~Game() = default;
	void Run() noexcept; // 游戏主循环，负责调用状态机的 update() 方法
private:
	bool init() noexcept; // 初始化 SDL 和游戏状态
	std::unique_ptr<StateMachine> stateMachine_;// 游戏状态机，管理当前状态并提供接口切换状态和更新状态
	bool running_{ true }; // 游戏运行标志，控制主循环退出
	std::unique_ptr<Resource> resource_; // 游戏资源，如地雷和旗子图形，使用 RAII 管理
};
