#pragma once
#include <cstdint>
#include <cassert>

enum Behaviour : uint32_t {
	NONE = 0,
	CAN_MOVE = 1 << 0,
	IS_PLAYER = 1 << 1,
	RESPOND_TO_INPUT = 1 << 2,
	CAN_WALK_THROUGH = 1<< 3,
	KILLS_PLAYER = 1 << 4,
	IS_HEAVY = 1 << 5,
	CAN_ROTATE = 1 << 6, 
	IS_PUSHING = 1 << 7,
	IS_HACKING = 1 << 8
};
enum class Direction {
	RIGHT,
	LEFT,
	UP,
	DOWN
};

inline Direction DirectionFromXY(int xDir, int yDir) {
	assert(xDir * yDir == 0);
	if (xDir == 1) { return Direction::RIGHT; }
	if (xDir == -1) { return Direction::LEFT; }
	if (yDir == 1) { return Direction::DOWN; }
	else { return Direction::UP; }
}
enum class ID : uint8_t {
	NONE = 0,
	WALL = 1,
	WALL_SHADOW = 2,
	WALL_LATERAL = 3,
	EXIT = 4,
	COMMAND_PANEL = 5,
	GRASS = 6,
	GRASS_SHADOW = 7,
	RUSTY_WALL = 8,
	YELLOW_BUTTON = 9,
	YELLOW_BUTTON_DOWN = 10,
	WATER_WITH_DIRT = 11,
	WATER_WITH_DIRT_SHADOW = 12,
	BELT_HORIZONTAL = 13,
	RED_BUTTON = 14,
	RED_BUTTON_DOWN = 15,
	WATER = 16,
	WATER_SHADOW = 17,
	BELT_LATERAL = 18,
	SPIKE = 19,
	SPIKE_DOWN = 20,
	BOX_1 = 21,
	BOX_2 = 22,
	BOX_GREEN = 23,
	BOX_METAL = 24,
	WATER_SHADOW_UP = 25,
	PLAYER = 27
};
struct Entity{
	ID id;
	int x;
	int y;
	Behaviour behaviour;
	int strength;
	Direction facing = Direction::DOWN;

	int x_prev;
	int y_prev;
	float progress_01;


};


struct LevelData;

bool IsMoving(Entity* e);
void AddEntity(ID id, int x, int y, LevelData* level);
void RemoveEntity(int x, int y, LevelData* level);

bool HasBehaviour(Entity* entity, Behaviour flags);
void SetBehaviour(Entity* entity, Behaviour flags);
void AddBehaviour(Entity* entity, Behaviour flags);
void RemoveBehaviour(Entity* entity, Behaviour flags);
void InitializeBaseBehaviour(Entity* entity);