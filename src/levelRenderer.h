#pragma once
#include "gameState.h"
#include "SDL3/SDL_render.h"

constexpr int TILESET_COLUMNS = 5;
constexpr int TILESET_TILE_PX = 16;
constexpr int TILESET_FIRSTGID = 1;

SDL_FRect GetTilesetSrcRect(int gid, int tilesetFirstGid, int columns, int tileSize);

void RenderLevel(Sokoban::GameData* gameData, SDL_Renderer* renderer);
void RenderEntities(Sokoban::GameData* gameData, SDL_Renderer* renderer);
void RenderDecorations(Sokoban::GameData* gameData, SDL_Renderer* renderer);