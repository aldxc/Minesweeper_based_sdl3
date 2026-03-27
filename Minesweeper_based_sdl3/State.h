#pragma once

// 状态机类型定义与简单封装，供各模块使用
enum class StateType { Menu, Playing, Won, Lost, Paused };

// 菜单动作：由坐标映射到菜单选择
enum class MenuAction { None, StartEasy, StartNormal, StartHard };

class StateMachine {
public:
    explicit StateMachine(StateType init = StateType::Menu) noexcept : state_(init) {}
    StateType get() const noexcept { return state_; }
    void set(StateType s) noexcept { state_ = s; }
    bool is(StateType s) const noexcept { return state_ == s; }

    // 根据窗口与鼠标坐标判断是否点击了菜单项（布局与 main/renderer 保持一致）
    MenuAction menuActionAt(int mx, int my, int winW, int winH) const noexcept {
        int btnW = 200, btnH = 50;
        int cx = winW / 2;
        int startY = winH / 2 - btnH - 10;
        struct Rect { int x; int y; int w; int h; };
        Rect easy{cx - btnW/2, startY, btnW, btnH};
        Rect normal{cx - btnW/2, startY + btnH + 10, btnW, btnH};
        Rect hard{cx - btnW/2, startY + 2*(btnH + 10), btnW, btnH};
        auto inside = [](const Rect& r, int x, int y){ return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h; };
        if (inside(easy, mx, my)) return MenuAction::StartEasy;
        if (inside(normal, mx, my)) return MenuAction::StartNormal;
        if (inside(hard, mx, my)) return MenuAction::StartHard;
        return MenuAction::None;
    }

private:
    StateType state_;
};
