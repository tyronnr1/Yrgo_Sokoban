// camera.cpp
#include "camera.h"
#include "common.h"

bool camera::GetIsPointInsideGrid(float x, float y, const LevelData* lvl, int screenW, int screenH) {
	int x_grid;
	int y_grid;
	WorldToGrid(x, y, &x_grid, &y_grid, lvl, screenW, screenH);
	return x_grid >= 0 && y_grid >= 0 && x_grid < lvl->w && y_grid < lvl->h;
}

void camera::GridToWorld(float* x, float* y, const LevelData* lvl, int screenW, int screenH) {
	*x *= CELL_SIZE_PX;
	*x += screenW / 2.0;
	*x -= lvl->w * CELL_SIZE_PX / 2.0;
	*y *= CELL_SIZE_PX;
	*y += screenH / 2.0;
	*y -= lvl->h * CELL_SIZE_PX / 2.0;
}

void camera::WorldToGrid(float x_world, float y_world, int* x, int* y, const LevelData* lvl, int screenW, int screenH) {
	*x = x_world;
	*y = y_world;
	*x += lvl->w * CELL_SIZE_PX / 2.0;
	*x -= screenW / 2.0;
	*x /= CELL_SIZE_PX;
	*y += lvl->h * CELL_SIZE_PX / 2.0;
	*y -= screenH / 2.0;
	*y /= CELL_SIZE_PX;
}