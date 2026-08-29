#include "rendering.h"
#include "SDL3/SDL_render.h"
#include "common.h"
#include "image.h"

void RenderSprite(Image* sprite, SDL_Renderer* renderer, int xPos, int yPos, float scale){
    SDL_FRect rect;
    rect.x = xPos;
    rect.y = yPos;
    rect.h = sprite->height * UPSCALE_FACTOR * scale;
    rect.w = sprite->width * UPSCALE_FACTOR * scale;
    SDL_RenderTexture(renderer, sprite->texture, NULL, &rect);
}

void RenderSprite(Image* sprite, SDL_Renderer* renderer, int xPos, int yPos, SDL_FRect srcRect, float scale){
    SDL_FRect dstRect;
    dstRect.x = xPos;
    dstRect.y = yPos;
    dstRect.h = srcRect.h * UPSCALE_FACTOR * scale;
    dstRect.w = srcRect.w * UPSCALE_FACTOR * scale;
    SDL_RenderTexture(renderer, sprite->texture, &srcRect, &dstRect);
}