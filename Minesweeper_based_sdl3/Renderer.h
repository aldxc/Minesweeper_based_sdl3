#pragma once

#include "Board.h"
#include <SDL3/SDL.h>
#include "SDL3_ttf/SDL_ttf.h"
#include <memory>
#include <string>
#include <string_view>

enum class StateType; // 前向声明，避免循环包含（实际在 State.h 中定义）

// Renderer: 负责 SDL 窗口及绘制，遵循 RAII（构造创建、析构释放）
class Renderer {
public:
    Renderer(int width, int height, const char* title);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = default;
    Renderer& operator=(Renderer&&) = default;

    void clear();//清除画布
	void present();//显示更新后的画布

    // 绘制棋盘；如果 tileSize==0 则自动根据窗口大小适配并居中
    // 现在接受可变 Board 引用以便在渲染后清理脏标记
    void drawBoard(Board& board, int tileSize = 0, int originX = 0, int originY = 0);

    // 绘制覆盖层（状态、计时器、剩余地雷数等）
    void drawOverlay(StateType state, int elapsedSeconds, int minesLeft);

    // 文本绘制：使用已加载字体，在指定位置绘制文本
    void drawText(const std::string_view& text, int x, int y, SDL_Color color, bool center = false);

    // 计算棋盘布局（当使用自动适配时）：输出 tileSize 与 origin
    void computeBoardLayout(const Board& board, int& outTileSize, int& outOriginX, int& outOriginY, int margin = 20);

    bool isInitialized() const noexcept { return initialized_; }

    // 访问底层 SDL 对象（必要时）以裸指针形式返回，不直接拥有
    SDL_Window* window() const noexcept { return window_.get(); }
    SDL_Renderer* renderer() const noexcept { return renderer_.get(); }

private:
    // 自定义删除器，用于 unique_ptr 管理 SDL 资源
    struct SDLWindowDeleter { void operator()(SDL_Window* w) const noexcept { if (w) SDL_DestroyWindow(w); } };
    struct SDLRendererDeleter { void operator()(SDL_Renderer* r) const noexcept { if (r) SDL_DestroyRenderer(r); } };
	struct SDLFontDeleter { void operator()(TTF_Font* f) const noexcept { if (f) TTF_CloseFont(f); } };

    // 使用智能指针管理 SDL 资源，符合 RAII
    std::unique_ptr<SDL_Window, SDLWindowDeleter> window_{nullptr};   // SDL 窗口（所有权）
    std::unique_ptr<SDL_Renderer, SDLRendererDeleter> renderer_{nullptr}; // SDL 渲染器（所有权）

    bool initialized_{false}; // 渲染器是否初始化成功

    // 字体
    std::unique_ptr<TTF_Font, SDLFontDeleter> font_{nullptr};
    int fontSize_{20};

    // 缓存上一次布局以判断是否需要重绘全盘
    int prevTileSize_{0};
    int prevOriginX_{0};
    int prevOriginY_{0};

    // 资源管理（后续可扩展纹理缓存、字体等）
    void initResources();
    void freeResources();
};
