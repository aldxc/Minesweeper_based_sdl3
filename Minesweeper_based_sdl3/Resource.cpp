#include <string>
#include "Resource.h"

std::array<SDL_Texture*, 13> textures_;

Resource::Resource(){
	//绑定数字纹理
    SDL_Texture* t = nullptr;
    for (int i = 0; i < 13; ++i) {
        if (i < 9) {
            t = getTexture();
            SDL_SetRenderTarget(Renderer::getInstance().getSDLRenderer(), t);
            SDL_RenderClear(Renderer::getInstance().getSDLRenderer()); // 清空纹理

            Renderer::getInstance().renderFillRect(SDL_FRect{ 0, 0, CELL_SIZE, CELL_SIZE }, SDL_Color(150, 150, 150, 255)); // 绘制打开的方块边框
            Renderer::getInstance().renderRect(SDL_FRect{ 0, 0, CELL_SIZE, CELL_SIZE }, SDL_Color(200, 200, 200, 255)); // 绘制边框
			if (i > 0)
                Renderer::getInstance().renderText(std::to_string(i), numberColors_[i], SDL_FRect{ 0, 0, CELL_SIZE, CELL_SIZE }, numSize); // 绘制数字到纹理
        }
        else {
            t = getTexture();
            SDL_SetRenderTarget(Renderer::getInstance().getSDLRenderer(), t);
            SDL_RenderClear(Renderer::getInstance().getSDLRenderer()); // 清空纹理

            if (i == 9) {
                Renderer::getInstance().renderFillRect(SDL_FRect{ 0, 0, CELL_SIZE, CELL_SIZE }, SDL_Color(100, 100, 100, 255)); // 绘制方块边框
                Renderer::getInstance().renderRect(SDL_FRect{ 0, 0, CELL_SIZE, CELL_SIZE }, SDL_Color(200, 200, 200, 255)); // 绘制边框
            }
            else if (i == 10) {
                Renderer::getInstance().renderFillRect(SDL_FRect{ 0, 0, CELL_SIZE, CELL_SIZE }, SDL_Color(150, 150, 150, 255)); // 绘制打开的方块边框
                Renderer::getInstance().renderRect(SDL_FRect{ 0, 0, CELL_SIZE, CELL_SIZE }, SDL_Color(200, 200, 200, 255)); // 绘制边框
            }
            else if (i == 11) {
                Renderer::getInstance().renderFillRect(SDL_FRect{ 0, 0, CELL_SIZE, CELL_SIZE }, SDL_Color(100, 100, 100, 255)); // 绘制方块边框
                Renderer::getInstance().renderRect(SDL_FRect{ 0, 0, CELL_SIZE, CELL_SIZE }, SDL_Color(200, 200, 200, 255)); // 绘制边框
                drawMine(Renderer::getInstance().getSDLRenderer(), SDL_FRect{ 0, 0, CELL_SIZE, CELL_SIZE }); // 绘制地雷
			}
            else if (i == 12) {
                Renderer::getInstance().renderFillRect(SDL_FRect{ 0, 0, CELL_SIZE, CELL_SIZE }, SDL_Color(100, 100, 100, 255)); // 绘制方块边框
                Renderer::getInstance().renderRect(SDL_FRect{ 0, 0, CELL_SIZE, CELL_SIZE }, SDL_Color(200, 200, 200, 255)); // 绘制边框
				drawFlag(Renderer::getInstance().getSDLRenderer(), SDL_FRect{ 0, 0, CELL_SIZE, CELL_SIZE }); // 绘制旗子
			}
        }

		textures_[i] = t; // 存储纹理指针
    }

    SDL_SetRenderTarget(Renderer::getInstance().getSDLRenderer(), nullptr); // 恢复默认渲染目标
}

inline void Resource::drawMine(SDL_Renderer* renderer, const SDL_FRect& rect) {
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

inline void Resource::drawFlag(SDL_Renderer* renderer, const SDL_FRect& rect) {
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
    int top = std::min({ fy1, fy2, fy3 });
    int bottom = std::max({ fy1, fy2, fy3 });
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
    SDL_FRect topRect{ poleX - 1, y + h / 6 - 2, 3, 3 };
    SDL_RenderFillRect(renderer, &topRect);
}

SDL_Texture* Resource::getTexture(){
    SDL_Texture* t = SDL_CreateTexture(Renderer::getInstance().getSDLRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, CELL_SIZE, CELL_SIZE); // 创建纹理，实际使用时需要检查返回值是否成功，并存储到 textures_ 中
    if (!t) {
		SDL_Log("Failed to create texture: %s", SDL_GetError());
        return nullptr;
    }
    return t;
}

