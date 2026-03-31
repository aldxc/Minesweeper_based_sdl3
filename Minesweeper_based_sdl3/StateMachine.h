#pragma once
#include <memory>
#include <stack>
#include "State.h"


// StateMachine: 管理当前状态并提供接口切换状态和更新状态
//game类已解耦，状态机只负责状态管理，游戏逻辑和输入处理由各个状态类实现
class StateMachine {
private:
	std::unique_ptr<State> currentState_;
	std::stack<std::unique_ptr<State>> stateStack_; // 状态栈，用于实现暂停状态的状态保存和恢复
public:
	StateMachine() : currentState_(createState({ StateType::Menu,0,0,0 })) {}
	std::unique_ptr<State> createState(State::StateTransInfo info); //工厂函数，根据 StateType 创建对应的 State 实例
	void changeStates(State::StateTransInfo info) {
		if (currentState_ && currentState_->is(info.state)) return; // 如果当前状态已经是目标状态，直接返回
		currentState_ = createState(info); // 创建新状态并替换当前状态
	}
	void update(SDL_Event& event) {
		auto newState = currentState_->update(event); // 调用当前状态的 update() 方法，获取可能的状态切换
		if (newState == std::nullopt) return; // 如果 update() 返回 std::nullopt，表示状态未改变，直接返回
		else {
			if (newState->state == StateType::Paused) stateStack_.push(std::move(currentState_));
			else if (currentState_->is(StateType::Paused) && newState->state != StateType::Paused) {
				if (newState->state == StateType::Playing && newState->mines == -1) { //使用特定标记判断是否从 Paused 状态切换回保存的 Playing 状态
					currentState_ = std::move(stateStack_.top());
					stateStack_.pop();
					return;
				}
				else {
					while (!stateStack_.empty()) {
						stateStack_.pop(); // 如果从 Paused 状态切换到非 Playing 状态，清空状态栈
					}
				}
			}
			changeStates(newState.value()); // 否则切换到新的状态
		}
	}
	void render() {
		Renderer::getInstance().beginRender();

		if (currentState_.get()->is(StateType::Paused) && !stateStack_.empty()) 
			stateStack_.top()->renderer(false); // 如果当前状态是 Paused 且状态栈不为空，渲染栈顶状态（即暂停前的状态）
		currentState_->renderer(); // 调用当前状态的 renderer() 方法进行渲染

		Renderer::getInstance().reDefaultAndPresent(); // 渲染完成后调用 reDefaultAndPresent() 刷新屏幕
	}
};

// 工厂函数，根据 StateType 创建对应的 State 实例
inline std::unique_ptr<State> StateMachine::createState(State::StateTransInfo info) {
	switch (info.state) {
	case StateType::Menu:
		return std::make_unique<MenuState>();
	case StateType::Playing:
		return std::make_unique<PlayingState>(info);
	case StateType::Won:
	case StateType::Lost:
		return std::make_unique<EndState>(info);
	case StateType::Paused:
		return std::make_unique<PausedState>(info);
	default:
		return std::make_unique<MenuState>();// 默认返回菜单状态
	}
}
