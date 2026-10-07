#include "levelRenderer.h"
#include "common.h" 
#include "rendering.h"   
#include "arena.h"
#include <algorithm>

using namespace Sokoban;


SDL_FRect GetTilesetSrcRect(int gid, int tilesetFirstGid, int columns, int tileSize) {
    int localId = gid - tilesetFirstGid;
    int col = localId % columns;
    int row = localId / columns;

    SDL_FRect rect;
    rect.x = (float)(col * tileSize);
    rect.y = (float)(row * tileSize);
    rect.w = (float)tileSize;
    rect.h = (float)tileSize;
    return rect;
}

int GetDrawLayer(ID id) {
    switch (id) {
    case ID::SPIKE:
    case ID::SPIKE_DOWN:
        return 0; // drawn first, i.e. "under"
    case ID::BOX_1:
    case ID::BOX_2:
    case ID::BOX_GREEN:
    case ID::BOX_METAL:
        return 1; // drawn after spikes, i.e. "on top"
    case ID::PLAYER:
        return 2; // player drawn last, on top of everything
    default:
        return 0;
    }
}

void RenderLevel(GameData* gameData, SDL_Renderer* renderer) {
    LevelData* lvl = gameData->scenes.gameplay.GetCurrentLevel();
    for (int x = 0; x < lvl->w; x++) {
        for (int y = 0; y < lvl->h; y++) {
            uint8_t cellType = lvl->GetCellID(x, y);
            if (cellType == 0) continue;

            SDL_FRect srcRect = GetTilesetSrcRect(cellType, TILESET_FIRSTGID, TILESET_COLUMNS, TILESET_TILE_PX);

            RenderSprite_Grid(gameData->tileset, lvl, renderer, &gameData->camera, (float)x, (float)y, srcRect, gameData->screenW, gameData->screenH);
        }
    }
}

bool IsEntityDrawOrderLess(Entity* a, Entity* b) {
    int layerA = GetDrawLayer(a->id);
    int layerB = GetDrawLayer(b->id);
    if (layerA != layerB) {
        return layerA < layerB;
    }
    return a->y < b->y;
}

void RenderEntities(GameData* data, SDL_Renderer* renderer) {
    LevelData* lvlData = data->scenes.gameplay.GetCurrentLevel();
    const int maxLayer = 2;

    Entity** sortedEntities = ALLOC_ARRAY(data->arena_scratch, Entity*, lvlData->entityCount);
    for (int i = 0; i < lvlData->entityCount; i++) {
        sortedEntities[i] = &lvlData->entityBuffer[i];
    }
    std::sort(sortedEntities, sortedEntities + lvlData->entityCount, IsEntityDrawOrderLess);

    for (int i = 0; i < lvlData->entityCount; i++) {
        Entity* entity = sortedEntities[i];
        if (entity->id == ID::NONE) {
            continue;
        }

        float x_animated = entity->x_prev + (entity->x - entity->x_prev) * entity->progress_01;
        float y_animated = entity->y_prev + (entity->y - entity->y_prev) * entity->progress_01;

        RenderEntity_OnTile(data->dropshadow, lvlData, renderer, &data->camera,
            x_animated, y_animated + 0.15f, data->screenW, data->screenH, 0.6f, 0.4f);

        if (entity->id == ID::PLAYER) {
            bool flip = entity->facing == Direction::LEFT;
            RenderEntity_OnTile(data->player, lvlData, renderer, &data->camera,
                x_animated, y_animated, data->screenW, data->screenH, 1, 1, flip);

            Gameplay* gameplay = &data->scenes.gameplay;
            if (gameplay->activePlayerBuffer != nullptr) {
                Entity* active = gameplay->activePlayerBuffer[gameplay->activePlayerIndex];
                if (active == entity) {
                    RenderEntity_OnTile(data->selectionMarker, lvlData, renderer, &data->camera,
                        x_animated, y_animated, data->screenW, data->screenH);
                }
            }
        }
        else {
            SDL_FRect srcRect = GetTilesetSrcRect(static_cast<int>(entity->id), TILESET_FIRSTGID, TILESET_COLUMNS, TILESET_TILE_PX);
            RenderEntity_OnTile(data->tileset, lvlData, renderer, &data->camera,
                x_animated, y_animated, srcRect, data->screenW, data->screenH);
        }
    }
}

void RenderDecorations(GameData* gameData, SDL_Renderer* renderer) {
    LevelData* lvl = gameData->scenes.gameplay.GetCurrentLevel();
    for (int x = 0; x < lvl->w; x++) {
        for (int y = 0; y < lvl->h; y++) {
            uint8_t cellType = lvl->GetDecorationID(x, y);
            if (cellType == 0) continue;

            SDL_FRect srcRect = GetTilesetSrcRect(cellType, TILESET_FIRSTGID, TILESET_COLUMNS, TILESET_TILE_PX);

            RenderSprite_Grid(gameData->tileset, lvl, renderer, &gameData->camera, (float)x, (float)y, srcRect, gameData->screenW, gameData->screenH);
        }
    }
}