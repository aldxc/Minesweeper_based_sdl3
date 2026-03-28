#pragma once
#include "State.h"
#include <memory>

// StateMachine: 管理当前状态并提供接口切换状态和更新状态
class StateMachine {
private:
	std::unique_ptr<State> currentState_;
public:
	StateMachine() : currentState_(createState(StateType::Menu)) {}
	std::unique_ptr<State> createState(StateType type); //工厂函数，根据 StateType 创建对应的 State 实例
	//game类已解耦，状态机只负责状态管理，游戏逻辑和输入处理由各个状态类实现
	void changeStates(StateType state) { 
		if (currentState_ && currentState_->is(state)) return; // 如果当前状态已经是目标状态，直接返回
		currentState_ = createState(state); // 创建新状态并替换当前状态
	}
	void update() { 
		auto newState = currentState_->update(); // 调用当前状态的 update() 方法，获取可能的状态切换
		if (newState == std::nullopt) return; // 如果 update() 返回 std::nullopt，表示状态未改变，直接返回
		else changeStates(newState.value()); // 否则切换到新的状态
	}
};

// 工厂函数，根据 StateType 创建对应的 State 实例
inline std::unique_ptr<State> StateMachine::createState(StateType type) {
	switch (type) {
	case StateType::Menu:
		return std::make_unique<MenuState>();
	case StateType::Playing:
		return std::make_unique<PlayingState>();
	case StateType::Won:
		return std::make_unique<WonState>();
	case StateType::Lost:
		return std::make_unique<LostState>();
	case StateType::Paused:
		return std::make_unique<PausedState>();
	default:
		return std::make_unique<MenuState>();// 默认返回菜单状态
	}
}
