#include "levelRenderer.h"
#include "common.h"      // CELL_SIZE_PX
#include "rendering.h"   // RenderSprite()
#include <cstdint>

using namespace Sokoban;

constexpr int TILESET_COLUMNS = 5;
constexpr int TILESET_TILE_PX = 16;
constexpr int TILESET_FIRSTGID = 1;

SDL_FRect GetTilesetSrcRect(int gid, int tilesetFirstGid, int columns, int tileSize){
    int localId = gid - tilesetFirstGid; // 0-indexed position within this tileset
    int col = localId % columns;
    int row = localId / columns;

    SDL_FRect rect;
    rect.x = (float)(col * tileSize);
    rect.y = (float)(row * tileSize);
    rect.w = (float)tileSize;
    rect.h = (float)tileSize;
    return rect;
}

void RenderLevel(GameData* gameData, SDL_Renderer* renderer){
    LevelData lvl = gameData->levels[gameData->currentLevel];
    int board_width_px_half = lvl.w * CELL_SIZE_PX / 2;
    int board_height_px_half = lvl.h * CELL_SIZE_PX / 2;
    for(int x = 0; x < lvl.w; x++){
        for (int y = 0 ; y < lvl.h; y++) {
            uint8_t cellType = lvl.GetCellID(x, y);
            if (cellType == 0) continue; // empty cell

            SDL_FRect srcRect = GetTilesetSrcRect(cellType, TILESET_FIRSTGID, TILESET_COLUMNS, TILESET_TILE_PX);

            float xPos = x * CELL_SIZE_PX;
            float yPos = y * CELL_SIZE_PX;
            xPos += gameData->screenW / 2.0;
            yPos += gameData->screenH / 2.0;
            xPos -= board_width_px_half;
            yPos -= board_height_px_half;
            RenderSprite(gameData->tileset, renderer, xPos, yPos, srcRect);
        }
    }
}

void RenderEntities(GameData* data, SDL_Renderer* renderer){
    LevelData lvlData = data->levels[data->currentLevel];
    for(int i = 0; i < lvlData.entityCount; i++){
        Entity entity = lvlData.entityBuffer[i];

        int xPos = 0;
        int yPos = 0;
        xPos += data->screenW / 2.0;
        yPos += data->screenH / 2.0;
        xPos -= data->levels[data->currentLevel].w * CELL_SIZE_PX / 2;
        yPos -= data->levels[data->currentLevel].h * CELL_SIZE_PX / 2;
        xPos += entity.x * CELL_SIZE_PX;
        yPos += entity.y * CELL_SIZE_PX;

        if (entity.id == 27) {
            // player has its own separate PNG - no source rect needed
            RenderSprite(data->player, renderer, xPos, yPos);
        } else {
            // boxes and everything else assumed to come from the Sokoban.tsx tileset
            SDL_FRect srcRect = GetTilesetSrcRect(entity.id, /*Sokoban.tsx firstgid*/ 26, TILESET_COLUMNS, TILESET_TILE_PX);
            RenderSprite(data->tileset, renderer, xPos, yPos, srcRect);
        }
    }
}