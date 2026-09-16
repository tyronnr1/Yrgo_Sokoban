#include "levelRenderer.h"
#include "common.h" 
#include "rendering.h"   

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
    LevelData lvl = gameData->levels[gameData->currentLevel];
    for (int x = 0; x < lvl.w; x++) {
        for (int y = 0; y < lvl.h; y++) {
            uint8_t cellType = lvl.GetCellID(x, y);
            if (cellType == 0) continue; // empty cell

            SDL_FRect srcRect = GetTilesetSrcRect(cellType, TILESET_FIRSTGID, TILESET_COLUMNS, TILESET_TILE_PX);

            RenderSprite_Grid(gameData->tileset, &lvl, renderer, &gameData->camera, (float)x, (float)y, srcRect, gameData->screenW, gameData->screenH);
        }
    }
}

void RenderEntities(GameData* data, SDL_Renderer* renderer) {
    LevelData lvlData = data->levels[data->currentLevel];
    const int maxLayer = 2;

    for (int layer = 0; layer <= maxLayer; layer++) {
        for (int i = 0; i < lvlData.entityCount; i++) {
            Entity entity = lvlData.entityBuffer[i];
            if (GetDrawLayer(entity.id) != layer) {
                continue;
            }

            float x_animated = entity.x_prev + (entity.x - entity.x_prev) * entity.progress_01;
            float y_animated = entity.y_prev + (entity.y - entity.y_prev) * entity.progress_01;

            if (entity.id == ID::PLAYER) {
                RenderSprite_Grid(data->player, &lvlData, renderer, &data->camera,  x_animated, y_animated, data->screenW, data->screenH);
            }
            else {
                SDL_FRect srcRect = GetTilesetSrcRect(static_cast<int>(entity.id), TILESET_FIRSTGID, TILESET_COLUMNS, TILESET_TILE_PX);
                RenderSprite_Grid(data->tileset, &lvlData, renderer, &data->camera, x_animated, y_animated, srcRect, data->screenW, data->screenH);
            }
        }
    }
}

void RenderDecorations(GameData* gameData, SDL_Renderer* renderer) {
    LevelData lvl = gameData->levels[gameData->currentLevel];
    for (int x = 0; x < lvl.w; x++) {
        for (int y = 0; y < lvl.h; y++) {
            uint8_t cellType = lvl.GetDecorationID(x, y);
            if (cellType == 0) continue; // empty cell

            SDL_FRect srcRect = GetTilesetSrcRect(cellType, TILESET_FIRSTGID, TILESET_COLUMNS, TILESET_TILE_PX);

            RenderSprite_Grid(gameData->tileset, &lvl, renderer, &gameData->camera, (float)x, (float)y, srcRect, gameData->screenW, gameData->screenH);
        }
    }
}