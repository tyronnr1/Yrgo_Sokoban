#include "rendering.h"
#include "SDL3/SDL_render.h"
#include "common.h"
#include "image.h"
#include <iostream>

void RenderSprite_World(Image* sprite, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale, float alpha, bool flipped) {
    SDL_FRect rect;
    rect.x = x;
    rect.y = y;
    rect.h = sprite->height * UPSCALE_FACTOR * scale;
    rect.w = sprite->width * UPSCALE_FACTOR * scale;
    rect.x -= camera->camera_x;
    rect.y -= camera->camera_y;

    SDL_SetTextureScaleMode(sprite->texture, SDL_SCALEMODE_PIXELART);
    SDL_SetTextureAlphaModFloat(sprite->texture, alpha);
    SDL_RenderTextureRotated(renderer, sprite->texture, NULL, &rect, 0.0, NULL,
        flipped ? SDL_FlipMode::SDL_FLIP_HORIZONTAL : SDL_FlipMode::SDL_FLIP_NONE);
}

void RenderSprite_World(Image* sprite, SDL_Renderer* renderer, const Camera* camera, float x, float y, SDL_FRect srcRect, float scale, float alpha, bool flipped) {
    SDL_FRect dstRect;
    dstRect.x = roundf(x);
    dstRect.y = roundf(y);
    dstRect.h = srcRect.h * UPSCALE_FACTOR * scale;
    dstRect.w = srcRect.w * UPSCALE_FACTOR * scale;
    dstRect.x -= camera->camera_x;
    dstRect.y -= camera->camera_y;

    SDL_SetTextureScaleMode(sprite->texture, SDL_SCALEMODE_PIXELART);
    SDL_SetTextureAlphaModFloat(sprite->texture, alpha);
    SDL_RenderTextureRotated(renderer, sprite->texture, &srcRect, &dstRect, 0.0, NULL,
        flipped ? SDL_FlipMode::SDL_FLIP_HORIZONTAL : SDL_FlipMode::SDL_FLIP_NONE);
}

void RenderSprite_Grid(Image* sprite, LevelData* lvl, SDL_Renderer* renderer, const Camera* camera, float x, float y, int screenW, int screenH, float scale, float alpha, bool flipped) {
    camera::GridToWorld(&x, &y, lvl, screenW, screenH);
    RenderSprite_World(sprite, renderer, camera, x, y, scale, alpha, flipped);
}

void RenderSprite_Grid(Image* sprite, LevelData* lvl, SDL_Renderer* renderer, const Camera* camera, float x, float y, SDL_FRect srcRect, int screenW, int screenH, float scale, float alpha, bool flipped) {
    camera::GridToWorld(&x, &y, lvl, screenW, screenH);
    RenderSprite_World(sprite, renderer, camera, x, y, srcRect, scale, alpha, flipped);
}

void RenderEntity_OnTile(Image* sprite, LevelData* lvl, SDL_Renderer* renderer, const Camera* camera, float x, float y, int screenW, int screenH, float scale, float alpha, bool flipped) {
    camera::GridToWorld(&x, &y, lvl, screenW, screenH);
    x += CELL_SIZE_PX / 2.0f;
    y += CELL_SIZE_PX / 2.0f;
    x -= sprite->pivot_x * UPSCALE_FACTOR * scale;
    y -= sprite->pivot_y * UPSCALE_FACTOR * scale;
    RenderSprite_World(sprite, renderer, camera, x, y, scale, alpha, flipped);
}

void RenderEntity_OnTile(Image* sprite, LevelData* lvl, SDL_Renderer* renderer, const Camera* camera, float x, float y, SDL_FRect srcRect, int screenW, int screenH, float scale, float alpha, bool flipped) {
    camera::GridToWorld(&x, &y, lvl, screenW, screenH);
    x += CELL_SIZE_PX / 2.0f;
    y += CELL_SIZE_PX / 2.0f;
    x -= (srcRect.w / 2.0f) * UPSCALE_FACTOR * scale;
    y -= (srcRect.h / 2.0f) * UPSCALE_FACTOR * scale;
    RenderSprite_World(sprite, renderer, camera, x, y, srcRect, scale, alpha, flipped);
}