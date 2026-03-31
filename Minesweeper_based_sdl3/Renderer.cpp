#include "Renderer.h"
#include <assert.h>
// 实现 Renderer 类方法

Renderer::Renderer(){
	//800，600默认初始化窗口，最好保存到constants中
	initResources("Minesweeper", 800, 600); // 初始化资源
}

Renderer::~Renderer() {
	//析构需要保证正确顺序
	// 先显式释放依赖于 SDL_ttf/SDL 的资源
	text_.reset();
	textEngine_.reset();
	font_.reset();
	renderer_.reset();
	window_.reset();

	TTF_Quit(); // 退出 SDL_ttf，释放相关资源
	SDL_Quit(); // 退出 SDL，释放所有资源
}

void Renderer::initResources(const char* title, int width, int height) noexcept{
	// 创建 SDL 窗口
	SDL_Window* w = SDL_CreateWindow(title, width, height, 0);	// 创建窗口
	if (!w) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return;
	}
	window_.reset(w); // 使用 unique_ptr 获取管理 SDL_Window 资源，确保异常安全

	// 创建 SDL 渲染器
	SDL_Renderer* r = SDL_CreateRenderer(window_.get(), nullptr); // 创建渲染器，关联到窗口
	if (!r) {
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return;
	}
	renderer_.reset(r); // 使用 unique_ptr 获取管理 SDL_Renderer 资源，确保异常安全

	TTF_Font* f = TTF_OpenFont("1.ttf", 30); // 加载字体，设置字体大小（默认可修改），路径为相对路径
	if (!f) {
		SDL_Log("Failed to load font: %s", SDL_GetError());
		return;
	}
	font_.reset(f); // 使用 unique_ptr 获取管理 TTF_Font 资源，确保异常安全

	TTF_TextEngine* te = TTF_CreateRendererTextEngine(renderer_.get()); // 创建文本引擎
	if (!te) {
		SDL_Log("Failed to create text engine: %s", SDL_GetError());
		return;
	}
	textEngine_.reset(te); // 使用 unique_ptr 获取管理 TTF_TextEngine 资源，确保异常安全

	boardUI_.reset(); // 初始化棋盘 UI 纹理，初始为 nullptr
}

void Renderer::renderRects(const std::vector<SDL_FRect>& rects, const SDL_Color& color) const noexcept{
	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a); // 设置绘制颜色
	SDL_RenderRects(renderer_.get(), rects.data(), static_cast<int>(rects.size())); // 批量绘制矩形边框
}

void Renderer::renderRect(const SDL_FRect& rect, const SDL_Color& color) const noexcept{
	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a); // 设置绘制颜色
	SDL_RenderRect(renderer_.get(), &rect); // 绘制矩形边框
}

void Renderer::renderFillRect(const SDL_FRect& rect, const SDL_Color& color) const noexcept{
	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a); // 设置绘制颜色
	SDL_RenderFillRect(renderer_.get(), &rect); // 绘制矩形
}

void Renderer::renderText(const std::string& text, const SDL_Color& color, const SDL_FRect& rect, const int textsize) noexcept{
	TTF_Text* t = TTF_CreateText(textEngine_.get(), font_.get(), text.c_str(), text.size()); // 创建文本对象
	if (!t) {
		SDL_Log("Failed to create text: %s", SDL_GetError());
		return;
	}
	TTF_SetFontSize(font_.get(), textsize); // 设置字体大小
	text_.reset(t); // 使用 unique_ptr 管理 TTF_Text 资源，确保异常安全
	TTF_SetTextColor(text_.get(), color.r, color.g, color.b, color.a); // 设置文本颜色
	int w = 0, h = 0;
	TTF_GetTextSize(text_.get(), &w, &h);
	//居中
	float x = rect.x + (rect.w - w) / 2.0f;
	float y = rect.y + (rect.h - h) / 2.0f;
	TTF_DrawRendererText(text_.get(), x, y); // 绘制文本到渲染器
}

void Renderer::renderTexts(const std::vector<std::string>& texts, const std::vector<SDL_FRect>& rects, const SDL_Color& color, const int textsize) noexcept{
	for(int i = 0; i < texts.size() && i < rects.size(); ++i) {
		renderText(texts[i], color, rects[i], textsize); // 逐个绘制文本
	}
}

void Renderer::renderDirtyBlocks(const std::vector<Board::transInfo>& blocks) const noexcept{

	int windowWidth = 0, windowHeight = 0;
	SDL_GetWindowSize(window_.get(), &windowWidth, &windowHeight);
	int rows = windowHeight / CELL_SIZE - UP_BLOCKS;//行数
	int cols = windowWidth / CELL_SIZE;//列数
	if (rows < 0) rows = 0;

	if(!blocks.empty()) updateTexture(blocks, rows, cols);

	for (int i = 0; i < dirtyBlockIndices_.size(); ++i) {
		int stateIdx = dirtyBlockIndices_[i];
		// 防御性检查：stateIdx 必须在 textures_ 范围内
		if (stateIdx < 0 || stateIdx >= static_cast<int>(textures_.size())) {
			SDL_Log("Invalid state index %d at dirtyBlockIndices_[%d]", stateIdx, i);
			continue;
		}
		auto tex = textures_[stateIdx];
		if (!tex) continue;

		SDL_FRect dstRect{
			(i % cols) * CELL_SIZE, ((i / cols) + UP_BLOCKS) * CELL_SIZE, CELL_SIZE, CELL_SIZE
		};
		SDL_RenderTexture(renderer_.get(), tex, nullptr, &dstRect);
	}
}



void Renderer::reDefaultAndPresent() const noexcept{
	SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, 255); // 恢复默认绘制颜色
	TTF_SetFontSize(font_.get(), 30); // 恢复字体大小
	SDL_RenderPresent(renderer_.get()); // 显示渲染结果，更新窗口内容
}

void Renderer::updateWindow(int width, int height) noexcept{
	SDL_SetWindowSize(window_.get(), width, height); // 调整窗口大小
	int cols = width / CELL_SIZE;
	int rows = height / CELL_SIZE - UP_BLOCKS;
	if (rows < 0) rows = 0;
	dirtyBlockIndices_.resize(cols * rows);
	// 将新的脏块数组初始化为“未掀开状态”（9），避免未初始化或默认 0 导致错误显示
	dirtyBlockIndices_.assign(cols * rows, 9);
}

void Renderer::saveBoardUI(const std::vector<SDL_FRect>& rects, const std::vector<std::string>& strings, int size, SDL_Color color) noexcept{
	int w = 0;
	SDL_GetWindowSize(window_.get(), &w, nullptr);
	SDL_Texture* t = SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, UP_BLOCKS * CELL_SIZE); // 创建目标纹理，大小为上方空白区域
	if (!t) {
		SDL_Log("Failed to create board UI texture: %s", SDL_GetError());
		return;
	}

	SDL_SetRenderTarget(renderer_.get(), t); // 设置渲染目标为 boardUI_ 纹理
	SDL_RenderClear(renderer_.get());
	renderTexts(strings, rects, color, size); // 在 boardUI_ 纹理上绘制文本
	SDL_SetRenderTarget(renderer_.get(), nullptr);

	boardUI_.reset(t); // 使用 unique_ptr 管理 SDL_Texture 资源，确保异常安全
}

void Renderer::renderBoardUI() const noexcept{
	int w = 0;
	SDL_GetWindowSize(window_.get(), &w, nullptr);
	SDL_FRect dstrect{ 0,0, w, UP_BLOCKS * CELL_SIZE };
	SDL_RenderTexture(renderer_.get(), boardUI_.get(), nullptr, &dstrect); // 绘制保存的棋盘 UI 纹理到窗口
}

void Renderer::updateTexture(const std::vector<Board::transInfo>& blocks, int rows, int cols) const noexcept{
	for(int i = 0; i < blocks.size(); ++i) {
		int index = blocks[i].row * cols + blocks[i].col;
		if(index >=0 && index < dirtyBlockIndices_.size())
			dirtyBlockIndices_[index] = blocks[i].state; // 更新脏格子索引列表
	}
}

