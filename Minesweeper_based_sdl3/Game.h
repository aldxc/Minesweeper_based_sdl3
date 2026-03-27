#pragma once

#include "Board.h"
#include "State.h"
#include <memory>
#include <chrono>

union SDL_Event; // 使用 union 前向声明以匹配 SDL 的定义，避免重定义

class Renderer; // 渲染器前置声明

class Game {
public:
    struct Params {
        int rows = 16;
        int cols = 30;
        int mines = 99;
        unsigned int seed = 0; // 0 表示使用随机设备生成
    };

    explicit Game(const Params& params = Params());
    ~Game() = default;

    // 启动/重置游戏
    void startNewGame(const Params& params);

    // 事件驱动接口：将来自 main 循环的 SDL_Event 翻译为游戏命令
    void handleEvent(const SDL_Event& ev);

    // 游戏逻辑更新（计时器、动画等）
    void update(std::chrono::milliseconds dt);

    // 只读访问，用于渲染
    const Board& getBoard() const noexcept { return board_; }
    Board& getBoard() noexcept { return board_; }
    StateType getState() const noexcept { return stateMachine_.get(); }
    int getElapsedSeconds() const noexcept { return elapsedSeconds_; }

    // 对外公开的点击接口（使用格子坐标）
    void onLeftClick(int r, int c);
    void onRightClick(int r, int c);
    void onChordClick(int r, int c); // 双击已打开格子揭示周围

    // 处理菜单点击：参数为鼠标坐标与窗口尺寸
    void handleMenuClick(int mx, int my, int winW, int winH);

private:
    Board board_;                // 棋盘逻辑实例（拥有所有格子数据）
    StateMachine stateMachine_{StateType::Menu}; // 状态机封装当前状态
    Params params_;              // 当前游戏参数（行/列/雷数/种子）
    bool firstClickHandled_{false}; // 是否已处理首个点击（用于首开安全）
    int elapsedSeconds_{0};      // 已用时间（秒）
    // 可扩展：计时器累加器等
};
