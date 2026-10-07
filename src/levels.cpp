#include <cstdint>
#include <fstream>
#include <vector>
#include "levels.h"
#include "arena.h"
#include "Parsers/json.hpp"
#include "entity.h"
#include <print>

using namespace std;

const int LEVEL_INDEX = 0;
const int ENTITIES_INDEX = 1;
const int DECORATIONS_INDEX = 2;
 


void CreateLevel(Arena* arena, LevelData* level, const char* level_name){

	fstream stream(level_name);
	auto jsonResult = nlohmann::json::parse(stream);
	vector<uint8_t> dataField = jsonResult["layers"][LEVEL_INDEX]["data"].get<vector<uint8_t>>();

	level->w = jsonResult["width"].get<int>();
	level->h = jsonResult["height"].get<int>();

	level->level_path = level_name;
	
	size_t size_of_cells = sizeof(uint8_t) * level->w * level->h;
	level->cells = (uint8_t*)Memory::Allocate(arena, size_of_cells);

	for (int i = 0; i < level->w * level->h; i++) {
		level->cells[i] = dataField[i];
	}
}

void CreateEntities(LevelData* lvl_data, Arena* arena){
	Reset(arena);

	lvl_data->entityCount = 0;
	fstream stream(lvl_data->level_path);

	auto result = nlohmann::json::parse(stream);
	auto entityData = result["layers"][ENTITIES_INDEX]["data"].get<vector<uint8_t>>();

	for (int i = 0; i < lvl_data->w * lvl_data->h; i++) {
		unsigned char entity_id = entityData[i];
		if(entity_id != 0){
			lvl_data->entityCount++;
		}
	}

	lvl_data->entityCapacity = lvl_data->entityCount + 32;

	lvl_data->entityBuffer = (Entity*)Memory::Allocate( arena, sizeof(Entity) * lvl_data->entityCapacity );

	int index = 0;
	for (int i = 0; i < lvl_data->w * lvl_data->h; i++) {

		unsigned char entity_id = entityData[i];
		
		if(entity_id != 0){
			int x = i % lvl_data->w;
			int y = i / lvl_data->w;

			lvl_data->entityBuffer[index].id = (ID)entity_id;
			InitializeBaseBehaviour(&lvl_data->entityBuffer[index]);
			lvl_data->entityBuffer[index].x = x;
			lvl_data->entityBuffer[index].y = y;
			lvl_data->entityBuffer[index].x_prev = x;
			lvl_data->entityBuffer[index].y_prev = y;
			lvl_data->entityBuffer[index].progress_01 = 0.0f;
			lvl_data->entityBuffer[index].active = true;

			index += 1;
		}
	}
}

void CreateDecorations(Arena* arena, LevelData* level, const char* level_name) {
	fstream stream(level_name);
	auto jsonResult = nlohmann::json::parse(stream);
	vector<uint8_t> dataField = jsonResult["layers"][DECORATIONS_INDEX]["data"].get<vector<uint8_t>>();

	level->w = jsonResult["width"].get<int>();
	level->h = jsonResult["height"].get<int>();

	level->level_path = level_name;

	size_t size_of_cells = sizeof(uint8_t) * level->w * level->h;
	level->decorations = (uint8_t*)Memory::Allocate(arena, size_of_cells);

	for (int i = 0; i < level->w * level->h; i++) {
		level->decorations[i] = dataField[i];
	}
}

Entity* RaycastFirstEntity(int x_origin, int y_origin, Direction direction, LevelData* level, bool ignore_walls) {
	int dx = 0, dy = 0;
	switch (direction) {
	case Direction::RIGHT: dx = 1;  dy = 0;  break;
	case Direction::LEFT:  dx = -1; dy = 0;  break;
	case Direction::UP:    dx = 0;  dy = -1; break;
	case Direction::DOWN:  dx = 0;  dy = 1;  break;
	}

	int x_search = x_origin + dx;
	int y_search = y_origin + dy;

	while (x_search >= 0 && x_search < level->w && y_search >= 0 && y_search < level->h) {
		ID cellID = (ID)level->GetCellID(x_search, y_search);
		if (cellID == ID::WALL && !ignore_walls) {
			break;
		}

		Entity* entity_search = level->GetEntity(x_search, y_search);
		if (entity_search != nullptr) {
			return entity_search;
		}

		x_search += dx;
		y_search += dy;
	}

	return nullptr;
}
