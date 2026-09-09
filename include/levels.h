#pragma once
#include "arena.h"
#include "entity.h"
#include <cstdint>
using namespace Memory;

struct LevelData{
	int w;
	int h;
	uint8_t* cells;
	const char* level_path;

	Entity* entityBuffer;
	int entityCount;
	
	uint8_t* decorations;

	uint8_t GetCellID(int x, int y){
		return cells[y * w + x];
	}

	Entity* GetEntity(int x, int y){
		for (int i = 0; i < entityCount; i++) {
			if(entityBuffer[i].x == x && entityBuffer[i].y == y){
				return &entityBuffer[i];
			}
		}
		return nullptr;
	}
	int GetEntitiesAt(int x, int y, Entity** out, int maxOut) {
		int count = 0;
		for (int i = 0; i < entityCount && count < maxOut; i++) {
			if (entityBuffer[i].x == x && entityBuffer[i].y == y) {
				out[count] = &entityBuffer[i];
				count++;
			}
		}
		return count;
	}
	uint8_t GetDecorationID(int x, int y) {
		return decorations[y * w + x];
	}

};

void CreateLevel(Arena* arena, LevelData* level, const char* level_name);
void CreateEntities(LevelData* lvl_data, Arena* arena);
void CreateDecorations(Arena* arena, LevelData* level, const char* level_name);
