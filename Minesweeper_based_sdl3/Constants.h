#pragma once
#include <array>

constexpr int CELL_SIZE = 40; // 每个格子的像素尺寸
constexpr int UP_BLOCKS = 5;//上方空闲区域像素尺寸
constexpr std::array<std::array<int, 3>, 3> BOARD_INFO = { {
    {10, 10, 20},
    {16, 16, 40},
    {24, 24, 99}
} };

extern std::array<SDL_Texture*, 13> textures_; // 预渲染的纹理：0-8 数字，9 方块，10 打开的方块，11 雷，12 旗子，13 矩形边框