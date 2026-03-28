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
	void changeStates(StateType state) { 
		if (currentState_ && currentState_->is(state)) return; // 如果当前状态已经是目标状态，直接返回
		currentState_ = createState(state); // 创建新状态并替换当前状态
	}
	void update() { currentState_.get()->update(); }
};
