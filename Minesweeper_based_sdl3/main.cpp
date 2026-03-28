#include "SDL3/SDL.h"
#include"SDL3/SDL_main.h"
#include "Game.h"

int SDL_main(int argc, char* argv[]) {
    // 主程序入口
	Game game; // 创建游戏实例，构造函数中会初始化 SDL 和游戏状态
	game.Run(); // 运行游戏主循环，直到用户退出
    return 0;
}