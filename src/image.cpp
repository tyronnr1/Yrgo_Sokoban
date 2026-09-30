#include <cassert>
#include <string>

#include "SDL3/SDL_render.h"
#include "SDL3_image/SDL_image.h"

#include "image.h"
#include "arena.h"
#include "gameState.h"

using namespace std;

const char* DIRECTORY = "assets/sprites/";
const char* FALLBACK = "assets/sprites/fallback.png";

Image* AssetManagement::LoadSprite(Memory::Arena* arena, SDL_Renderer* renderer, const char* name, int pivot_x, int pivot_y) {
    string path = DIRECTORY;
    path.append(name);
    SDL_Surface* surface = IMG_Load(path.c_str());

    if (surface == nullptr) {
        surface = IMG_Load(FALLBACK);
    }

    assert(surface != nullptr);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    Image* img = (Image*)Memory::Allocate(arena, sizeof(Image));

    img->texture = texture;
    img->height = texture->h;
    img->width = texture->w;

    if (pivot_x < 0 || pivot_y < 0) {
        img->pivot_x = img->width / 2;
        img->pivot_y = img->height / 2;
    }
    else {
        img->pivot_x = pivot_x;
        img->pivot_y = pivot_y;
    }

    SDL_DestroySurface(surface);
    return img;
}

void AssetManagement::LoadAllSprites(Sokoban::GameData* data, SDL_Renderer* renderer) {
    data->fallback = LoadSprite(data->arena_images, renderer, "fallback.png");
    data->tileset = LoadSprite(data->arena_images, renderer, "tileset.png");
    data->player = LoadSprite(data->arena_images, renderer, "player.png");
    data->dropshadow = LoadSprite(data->arena_images, renderer, "drop_shadow.png");
}