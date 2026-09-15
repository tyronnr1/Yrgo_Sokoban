#include "rendering.h"
#include "SDL3/SDL_render.h"
#include "common.h"
#include "image.h"
#include <iostream>

void RenderSprite_World(Image* sprite, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale) {
    SDL_FRect rect;
    rect.x = x;
    rect.y = y;
    rect.h = sprite->height * UPSCALE_FACTOR * scale;
    rect.w = sprite->width * UPSCALE_FACTOR * scale;
    rect.x -= camera->camera_x;
    rect.y -= camera->camera_y;

    SDL_RenderTexture(renderer, sprite->texture, NULL, &rect);
}

void RenderSprite_World(Image* sprite, SDL_Renderer* renderer, const Camera* camera, float x, float y, SDL_FRect srcRect, float scale) {
    SDL_FRect dstRect;
    dstRect.x = roundf(x);
    dstRect.y = roundf(y);
    dstRect.h = srcRect.h * UPSCALE_FACTOR * scale;
    dstRect.w = srcRect.w * UPSCALE_FACTOR * scale;
    dstRect.x -= camera->camera_x;
    dstRect.y -= camera->camera_y;

    SDL_RenderTexture(renderer, sprite->texture, &srcRect, &dstRect);
}

void RenderSprite_Grid(Image* sprite, LevelData* lvl, SDL_Renderer* renderer, const Camera* camera, float x, float y, int screenW, int screenH, float scale) {
    camera::GridToWorld(&x, &y, lvl, screenW, screenH);
    RenderSprite_World(sprite, renderer, camera, x, y, scale);
}

void RenderSprite_Grid(Image* sprite, LevelData* lvl, SDL_Renderer* renderer, const Camera* camera, float x, float y, SDL_FRect srcRect, int screenW, int screenH, float scale) {
    
    camera::GridToWorld(&x, &y, lvl, screenW, screenH);
    RenderSprite_World(sprite, renderer, camera, x, y, srcRect, scale);
}