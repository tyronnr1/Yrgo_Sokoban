#pragma once
#include <cstdint>
#include <cassert>

enum Behaviour : uint32_t {
	NONE = 0,
	CAN_MOVE = 1 << 0,
	IS_PLAYER = 1 << 1,
	RESPOND_TO_INPUT = 1 << 2,
	CAN_WALK_THROUGH = 1<< 3,
	KILLS_PLAYER = 1 << 4
};

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

	int x_prev;
	int y_prev;
	float progress_01;

	bool HasBehaviour(Behaviour flags){
		return (behaviour & flags) == flags;
	}
	void SetBehaviour(Behaviour flags){
		behaviour = flags;
	}
	void AddBehaviour(Behaviour flags){
		behaviour = (Behaviour)(behaviour | flags);
	}
	void RemoveBehaviour(Behaviour flags){
		behaviour = (Behaviour)(behaviour & ~flags);
	}
	
	void InitializeBaseBehaviour(){
		assert(id != ID::NONE);
		switch (id) {
			default:
				SetBehaviour(NONE);
			break;
			case ID::PLAYER:
				SetBehaviour((Behaviour)(CAN_MOVE | IS_PLAYER | RESPOND_TO_INPUT));
			break;
			case ID::BOX_1:
				SetBehaviour((Behaviour)(CAN_MOVE));
				break;
			case ID::SPIKE_DOWN:
				SetBehaviour((Behaviour)(CAN_WALK_THROUGH));
				break;
			case ID::SPIKE:
				SetBehaviour((Behaviour)(CAN_WALK_THROUGH | KILLS_PLAYER));
				break;
		}
	}
};

bool IsMoving(Entity* e);