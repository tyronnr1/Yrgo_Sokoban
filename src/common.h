#pragma once
#include <cstddef>

#define KILOBYTES(n) ((size_t)(n) * 1024)
#define MEGABYTES(n) (KILOBYTES(n) * 1024)
#define GIGABYTES(n) (MEGABYTES(n) * 1024)

constexpr size_t GAME_MEMORY_ALLOWANCE = MEGABYTES(5);

constexpr int FPS = 60;
const double FRAME_TIME_MS = 1000.0 / FPS;

const int UPSCALE_FACTOR = 3;
const int CELL_SIZE_PX = 16 * UPSCALE_FACTOR;