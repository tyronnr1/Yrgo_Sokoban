#pragma once
#include "SDL3/SDL_render.h"
#include "arena.h"

struct Image {
    SDL_Texture* texture;
    int width;
    int height;
    int pivot_x;
    int pivot_y;
};

namespace Sokoban { struct GameData; }

namespace AssetManagement {
    Image* LoadSprite(Memory::Arena* arena, SDL_Renderer* renderer, const char* path, int pivot_x = -1, int pivot_y = -1);
    void LoadAllSprites(Sokoban::GameData* data, SDL_Renderer* renderer);
}