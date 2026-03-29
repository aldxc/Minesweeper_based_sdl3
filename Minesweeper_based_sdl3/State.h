#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <utility>
#include <vector>
#include <optional>
#include "Renderer.h"
#include "Board.h"

// 状态
enum class StateType { Menu, Playing, Won, Lost, Paused };

class State {
public:
    struct StateTransInfo {
        StateType state;
        int rows;
        int cols;
        int mines;
	};
    explicit State(StateType init = StateType::Menu) noexcept : state_(init) {}
	virtual ~State() = default;

    //禁用拷贝
    State(const State& sm) = delete;
    State& operator=(const State& sm) = delete;
    State(State&&) = delete;
    State& operator=(State&&) = delete;

    StateType get() const noexcept { return state_; }
	//void set(StateType s) noexcept { state_ = s; }// 状态机的状态切换由 StateMachine 负责，State 类只提供接口查询当前状态
    bool is(StateType s) const noexcept { return state_ == s; }
    virtual void renderer() const noexcept = 0; // 状态渲染
	virtual std::optional<StateTransInfo> update(SDL_Event& event) noexcept = 0; // 更新状态机内部逻辑 handleInput() 处理输入事件，update() 处理状态更新（如动画、计时器等）

private:
    StateType state_;
};

//-----------------------------MenuState-----------------------------//
class MenuState : public State {
public:
    explicit MenuState() noexcept;
	~MenuState() noexcept override = default;
    void renderer() const noexcept override final;
    std::optional<StateTransInfo> update(SDL_Event& event) noexcept override final;
private:
	std::vector<SDL_FRect> buttonRects_; // 存储按钮 east-muedium-hard 
	std::vector<std::string> buttonLabels_; // 存储按钮标签
	SDL_FRect gameNameRect_{ 0, 0, 0, 0 }; // 存储游戏名称位置和大小
	int buttonCount_{ 3 }; // 按钮数量
	std::pair<int, int> windowSize_{ 0, 0 }; // 窗口尺寸，用于计算按钮位置和大小
};

//-----------------------------PlayingState-----------------------------//
class PlayingState : public State {
public:
    explicit PlayingState(StateTransInfo info) noexcept;
    void renderer() const noexcept override final;
    std::optional<StateTransInfo> update(SDL_Event& event) noexcept override final;
private:
	Board board_; // 扫雷棋盘逻辑
};

//-----------------------------WonState-----------------------------//
class WonState : public State {
public:
    explicit WonState() noexcept : State(StateType::Won) {}
    void renderer() const noexcept override final;
    std::optional<StateTransInfo> update(SDL_Event& event) noexcept override final;
};

//-----------------------------LostState-----------------------------//
class LostState : public State {
public:
    explicit LostState() noexcept : State(StateType::Lost) {}
    void renderer() const noexcept override final;
    std::optional<StateTransInfo> update(SDL_Event& event) noexcept override final;
};

//-----------------------------PausedState-----------------------------//
class PausedState : public State {
    //状态栈
public:
    explicit PausedState() noexcept : State(StateType::Paused) {}
    void renderer() const noexcept override final;
    std::optional<StateTransInfo> update(SDL_Event& event) noexcept override final;
};