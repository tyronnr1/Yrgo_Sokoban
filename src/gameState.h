#pragma once
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "arena.h"
#include "levels.h"

struct Image;

namespace Sokoban
{
    struct Vector2
    {
        float x;
        float y;
    };
    struct GameData
    {
        int screenW;
        int screenH;
        
        float currentFPS;

        Image* fallback;
        Image* wall;
        Image* ground;
        Image* player;
        Image* tileset;

        Memory::Arena* arena_levels;
        Memory::Arena* arena_entities;
        Memory::Arena* arena_images;

        LevelData* levels;
        
        int levelCount;
        int currentLevel;
    };
}