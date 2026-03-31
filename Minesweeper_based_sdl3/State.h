#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <utility>
#include <vector>
#include <optional>
#include <chrono>
#include "Renderer.h"
#include "Board.h"

// 状态
enum class StateType { Menu, Playing, Won, Lost, Paused };

//-----------------------------AbstractStateBase-----------------------------//
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
    bool is(StateType s) const noexcept { return state_ == s; }
    virtual void renderer(bool flag = true) const noexcept = 0; // 状态渲染
	virtual std::optional<StateTransInfo> update(SDL_Event& event) noexcept = 0; // 更新状态机内部逻辑 handleInput() 处理输入事件，update() 处理状态更新

private:
    StateType state_;
};

//-----------------------------MenuState-----------------------------//
class MenuState : public State {
public:
    explicit MenuState() noexcept;
	~MenuState() noexcept override = default;
    void renderer(bool flag = true) const noexcept override final;
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
    void renderer(bool flag = true) const noexcept override final;
    std::optional<StateTransInfo> update(SDL_Event& event) noexcept override final;
    void updateTimer(bool flag) const noexcept;
private:
    Board board_; // 扫雷棋盘逻辑
    std::vector<SDL_FRect> upBlocks_; //重开，计时器，雷数，暂停等
    mutable float timer_{ 0 }; // 计时器，单位为秒
    mutable std::chrono::time_point< std::chrono::high_resolution_clock> lastTime_{ std::chrono::high_resolution_clock::now() }; // 上次更新时间点，用于计算增量时间

    //test
	SDL_FRect tempWin{ 0, 0, 0, 0 };// 用于测试胜利条件的临时格子
};

//-----------------------------EndState-----------------------------//
class EndState : public State { // 包含胜利和失败两种结束状态，根据传入的 StateTransInfo 中的 state 字段区分
public:
    explicit EndState(StateTransInfo info) noexcept;
    void renderer(bool flag = true) const noexcept override final;
    std::optional<StateTransInfo> update(SDL_Event& event) noexcept override final;
private:
	SDL_FRect winMessageRect_{ 0, 0, 0, 0 }; // 存储胜利消息位置和大小
	SDL_FRect restartButtonRect_{ 0, 0, 0, 0 }; // 存储重开按钮位置和大小
	SDL_FRect backToMenuButtonRect_{ 0, 0, 0, 0 }; // 存储返回菜单按钮位置和大小

    StateTransInfo info_;
};

//-----------------------------PausedState-----------------------------//
class PausedState : public State {
public:
    explicit PausedState(StateTransInfo info) noexcept;
    void renderer(bool flag = true) const noexcept override final;
    std::optional<StateTransInfo> update(SDL_Event& event) noexcept override final;
private:
	std::vector<SDL_FRect> buttonRects_; // 存储按钮位置和大小
    StateTransInfo info_;
};