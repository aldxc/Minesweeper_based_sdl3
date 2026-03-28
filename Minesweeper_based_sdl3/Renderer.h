#pragma once

#include "Board.h"
#include <SDL3/SDL.h>
#include "SDL3_ttf/SDL_ttf.h"
#include <memory>
#include <string>
#include <string_view>

//单例模式的 Renderer 类，负责 SDL 窗口及绘制，遵循 RAII（构造创建、析构释放）

enum class StateType; // 前向声明，避免循环包含（实际在 State.h 中定义）

// Renderer: 负责 SDL 窗口及绘制，遵循 RAII（构造创建、析构释放）
class Renderer {
public:
	Renderer() = default;
    static Renderer& getInstance() {
		static Renderer instance; // 局部静态变量，线程安全的单例实现
		return instance;
    }
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = default;
    Renderer& operator=(Renderer&&) = default;


private:

};
