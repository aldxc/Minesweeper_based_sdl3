#pragma once

#include <SDL3/SDL.h>
#include "SDL3_ttf/SDL_ttf.h"
#include <memory>
#include <vector>
#include <string>

//单例模式的 Renderer 类，负责 SDL 窗口及绘制，遵循 RAII（构造创建、析构释放）

//enum class StateType; // 前向声明，避免循环包含（实际在 State.h 中定义）

// Renderer: 负责 SDL 窗口及绘制，遵循 RAII（构造创建、析构释放）
class Renderer {
public:
    Renderer();
    static Renderer& getInstance() {
		static Renderer instance; // 局部静态变量，线程安全的单例实现
		return instance;
    }
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = default;
    Renderer& operator=(Renderer&&) = default;

	void initResources(const char* title, int width, int height) noexcept; // 初始化 SDL 窗口和渲染器等资源
    void renderRects(const std::vector<SDL_FRect>& rects, const SDL_Color& color) const noexcept;
	void renderRect(const SDL_FRect& rect, const SDL_Color& color = SDL_Color(100,100,100,0)) const noexcept; // 绘制矩形
	void renderText(const std::string& text, const SDL_Color& color, const SDL_FRect& rect) noexcept; // 绘制文本
	void renderTexts(const std::vector<std::string>& texts, const std::vector<SDL_FRect>& rects, const SDL_Color& color) noexcept; // 批量绘制文本
    void beginRender() const noexcept { SDL_RenderClear(renderer_.get()); } // 开始渲染，清空屏幕
	void reDefaultAndPresent() const noexcept; // 恢复默认绘制并显示渲染结果

private:
	// 自定义删除器
    struct SDL_WINDOW_Deleter {
        void operator()(SDL_Window* window) const noexcept {
            if (window) SDL_DestroyWindow(window); // SDL 窗口销毁
        }
	};
    struct SDL_RENDERER_Deleter {
        void operator()(SDL_Renderer* renderer) const noexcept {
            if (renderer) SDL_DestroyRenderer(renderer); // SDL 渲染器销毁
        }
	};
    struct TTF_FONT_Deleter {
        void operator()(TTF_Font* font) const noexcept {
            if (font) TTF_CloseFont(font); // SDL_ttf 字体销毁
        }
    };
    struct TTF_TEXTENGINE_Deleter {
        void operator()(TTF_TextEngine* textEngine) const noexcept {
            if (textEngine) TTF_DestroyRendererTextEngine(textEngine); // SDL_ttf 文本引擎销毁
        }
	};
    struct TTF_TEXT_Deleter {
        void operator()(TTF_Text* text) const noexcept {
            if (text) TTF_DestroyText(text); // SDL_ttf 文本对象销毁
        }
	};
	std::unique_ptr<SDL_Window, SDL_WINDOW_Deleter> window_; // SDL 窗口
	std::unique_ptr<SDL_Renderer, SDL_RENDERER_Deleter> renderer_; // SDL 渲染器
	std::unique_ptr<TTF_Font, TTF_FONT_Deleter> font_; // SDL_ttf 字体
	std::unique_ptr<TTF_TextEngine, TTF_TEXTENGINE_Deleter> textEngine_; // SDL_ttf 文本引擎
	std::unique_ptr<TTF_Text, TTF_TEXT_Deleter> text_; // SDL_ttf 文本对象

	//脏格子纹理texture_，用于优化渲染，仅重绘改变的格子
};
