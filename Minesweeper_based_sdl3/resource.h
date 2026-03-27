#pragma once

#include <SDL3/SDL.h>
#include <cmath>

// resource.h 提供基于线段/像素绘制的简单图形：旗子与地雷
// 所有函数为 inline，直接使用 SDL_Renderer* 绘制到屏幕

inline void drawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius) {
    // 使用水平线段填充算法绘制实心圆
    for (int dy = -radius; dy <= radius; ++dy) {
        int dx = static_cast<int>(std::sqrt(radius * radius - dy * dy));
		SDL_RenderLine(renderer, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

inline void drawMine(SDL_Renderer* renderer, const SDL_FRect& rect) {
    // 在给定 rect 中心绘制地雷（黑圆 + 放射线）
    int cx = rect.x + rect.w / 2;
    int cy = rect.y + rect.h / 2;
    int r = std::min(rect.w, rect.h) / 3; // 半径

    // 圆心填充（黑色）
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    drawFilledCircle(renderer, cx, cy, r);

    // 放射状的线（灰色）表示地雷触角
    SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
    const int spokes = 8;
    const double PI = std::acos(-1.0);
    for (int i = 0; i < spokes; ++i) {
        double angle = (2.0 * PI * i) / spokes;
        int x2 = static_cast<int>(cx + (r + 6) * std::cos(angle));
        int y2 = static_cast<int>(cy + (r + 6) * std::sin(angle));
		SDL_RenderLine(renderer, cx, cy, x2, y2);
    }
}

inline void drawFlag(SDL_Renderer* renderer, const SDL_FRect& rect) {
    // 在给定 rect 中绘制一个简易的旗子（旗杆 + 三角形旗帜）
    int x = rect.x;
    int y = rect.y;
    int w = rect.w;
    int h = rect.h;

    // 旗杆（深棕色）
    SDL_SetRenderDrawColor(renderer, 101, 67, 33, 255);
    int poleX = x + w / 4;
	SDL_RenderLine(renderer, poleX, y + h / 6, poleX, y + (5 * h) / 6);

    // 旗帜（红色）：用三角形近似（两条线）
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    int fx1 = poleX;
    int fy1 = y + h / 6;
    int fx2 = x + (3 * w) / 4;
    int fy2 = y + h / 3;
    int fx3 = poleX;
    int fy3 = y + h / 2;
    // 绘制三角形的边界（填充可通过水平线段实现）
    // 简单填充：对每一行计算左右边界并绘制
    int top = std::min({fy1, fy2, fy3});
    int bottom = std::max({fy1, fy2, fy3});
    for (int yy = top; yy <= bottom; ++yy) {
        // 线性插值计算左右边界（简单方法，足够用于小旗帜）
        // 计算与两边线段的交点并取最小/最大
        // 为简便使用固定填充宽度
        int left = std::min(fx1, fx3);
        int right = fx2;
		SDL_RenderLine(renderer, left, yy, right, yy);
    }

    // 旗杆顶部小方块（黑色）
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_FRect topRect{poleX - 1, y + h / 6 - 2, 3, 3};
    SDL_RenderFillRect(renderer, &topRect);
}
