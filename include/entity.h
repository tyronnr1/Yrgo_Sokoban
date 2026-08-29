#pragma once
#include <cstdint>
#include <cassert> // So we can use assert()

struct Entity{
	uint8_t id;
	int x;
	int y;
};

enum Behaviour : uint32_t {
	NONE = 0,
	CAN_MOVE = 1 << 0,
	IS_PLAYER = 1 << 1,
	RESPOND_TO_INPUT = 1 << 2
};

enum class ID : uint8_t {
	WALL = 1,
	WALL_SHADOW = 2,
	WALL_LATERAL = 3,
	PLAYER_SPAWN = 4,
	EXIT = 5,
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
