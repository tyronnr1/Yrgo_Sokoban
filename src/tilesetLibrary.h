#pragma once
#include <cstdint>

namespace Memory { struct Arena; }

enum TileFlag : uint32_t {
    TILE_NONE = 0,
    TILE_WALKABLE = 1 << 0,
    TILE_KILLS_PLAYER = 1 << 1,
    TILE_IS_EXIT = 1 << 2,
    TILE_IS_COMMAND_PANEL = 1 << 3,
};

namespace AssetManagement {
    void LoadTileProperties(uint32_t** out_buffer, int* out_count, Memory::Arena* arena, const char* tsj_path);
}

bool TileHasFlag(uint32_t* tileFlags, int local_tile_id, TileFlag flag);