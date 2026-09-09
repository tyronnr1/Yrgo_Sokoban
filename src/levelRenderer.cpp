#include "levelRenderer.h"
#include "common.h" 
#include "rendering.h"   

using namespace Sokoban;

constexpr int TILESET_COLUMNS = 5;
constexpr int TILESET_TILE_PX = 16;
constexpr int TILESET_FIRSTGID = 1;

SDL_FRect GetTilesetSrcRect(int gid, int tilesetFirstGid, int columns, int tileSize){
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

void RenderEntities(GameData* data, SDL_Renderer* renderer) {
    LevelData lvlData = data->levels[data->currentLevel];
    const int maxLayer = 2;

    for (int layer = 0; layer <= maxLayer; layer++) {
        for (int i = 0; i < lvlData.entityCount; i++) {
            Entity entity = lvlData.entityBuffer[i];
            if (GetDrawLayer(entity.id) != layer) {
                continue;
            }

            int xPos = 0;
            int yPos = 0;

            xPos += data->screenW / 2.0;
            yPos += data->screenH / 2.0;

            xPos -= data->levels[data->currentLevel].w * CELL_SIZE_PX / 2;
            yPos -= data->levels[data->currentLevel].h * CELL_SIZE_PX / 2;



            float x_animated = entity.x_prev + (entity.x - entity.x_prev) * entity.progress_01;            
            float y_animated = entity.y_prev + (entity.y - entity.y_prev) * entity.progress_01;

            xPos += x_animated * CELL_SIZE_PX;
            yPos += y_animated * CELL_SIZE_PX; 

            if (entity.id == ID::PLAYER) {
                RenderSprite(data->player, renderer, xPos, yPos);
            }
            else {
                // boxes and everything else assumed to come from the Sokoban.tsx tileset
                SDL_FRect srcRect = GetTilesetSrcRect(static_cast<int>(entity.id), TILESET_FIRSTGID, TILESET_COLUMNS, TILESET_TILE_PX);
                RenderSprite(data->tileset, renderer, xPos, yPos, srcRect);
            }
        }
    }
}
void RenderDecorations(GameData* gameData, SDL_Renderer* renderer) {
    LevelData lvl = gameData->levels[gameData->currentLevel];
    int board_width_px_half = lvl.w * CELL_SIZE_PX / 2;
    int board_height_px_half = lvl.h * CELL_SIZE_PX / 2;
    for (int x = 0; x < lvl.w; x++) {
        for (int y = 0; y < lvl.h; y++) {
            uint8_t cellType = lvl.GetDecorationID(x, y);
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