#pragma once
#include "gameState.h"
#include "SDL3/SDL_render.h"

void RenderLevel(Sokoban::GameData* gameData, SDL_Renderer* renderer);
void RenderEntities(Sokoban::GameData* gameData, SDL_Renderer* renderer);