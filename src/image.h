#pragma once
#include "SDL3/SDL_render.h"
#include "arena.h"

struct Image {
    SDL_Texture* texture;
    int width;
    int height;
};

namespace Sokoban { struct GameData; }

namespace AssetManagement {
    Image* LoadSprite(Memory::Arena* arena, SDL_Renderer* renderer, const char* path);
    void LoadAllSprites(Sokoban::GameData* data, SDL_Renderer* renderer);
}