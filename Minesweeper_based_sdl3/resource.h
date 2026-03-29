#pragma once

#include <SDL3/SDL.h>
#include <cmath>
#include <array>
#include "Renderer.h"

// resource.h 提供基于线段/像素绘制的简单图形：旗子与地雷
// 所有函数为 inline，直接使用 SDL_Renderer* 绘制到屏幕
//Resource 借用 Renderer 的 SDL 上下文创建资源
class Resource {
public:
    Resource();
	~Resource() = default;
private:
    void drawMine(SDL_Renderer* renderer, const SDL_FRect& rect); // 绘制地雷
	void drawFlag(SDL_Renderer* renderer, const SDL_FRect& rect); // 绘制旗子
    inline void drawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius) {
        // 使用水平线段填充算法绘制实心圆
        for (int dy = -radius; dy <= radius; ++dy) {
            int dx = static_cast<int>(std::sqrt(radius * radius - dy * dy));
            SDL_RenderLine(renderer, cx - dx, cy + dy, cx + dx, cy + dy);
        }
    }

    std::array<SDL_Color, 9> numberColors_ = { // 数字颜色，1-8 分别为蓝绿红紫深蓝绿红黑
        SDL_Color(0, 0, 255, 255),    // 1 - 蓝色
        SDL_Color(0, 128, 0, 255),    // 2 - 绿色
        SDL_Color(255, 0, 0, 255),    // 3 - 红色
        SDL_Color(128, 0, 128, 255),  // 4 - 紫色
        SDL_Color(0, 0, 128, 255),    // 5 - 深蓝色
        SDL_Color(0, 128, 128, 255),  // 6 - 青色
        SDL_Color(255, 0, 255, 255),   // 7 - 洋红色
        SDL_Color(0, 0, 0, 255)       // 8 - 黑色
	};
	const int numSize = 30; // 数字文本大小
    SDL_Texture* getTexture();
};



