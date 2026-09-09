#pragma once

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_events.h"
#include "gameState.h"

extern "C"
{
    __declspec(dllexport) void Initialize(Sokoban::GameData* data, SDL_Window* window, SDL_Renderer* renderer);
    __declspec(dllexport) bool HandleEvents(Sokoban::GameData* data, SDL_Event event);
    __declspec(dllexport) void Update(Sokoban::GameData* data, float dt);
    __declspec(dllexport) void Draw(Sokoban::GameData* data, SDL_Renderer* renderer);
    __declspec(dllexport) void OnQuit(SDL_Renderer* renderer);
}
