#include "tilesetLibrary.h"
#include "Parsers/json.hpp"
#include "arena.h"
#include <fstream>
#include <cassert>

using namespace nlohmann;
using namespace std;

static TileFlag FlagFromPropertyName(const string& name) {
    if (name == "walkable") return TILE_WALKABLE;
    if (name == "kills_player") return TILE_KILLS_PLAYER;
    if (name == "is_exit") return TILE_IS_EXIT;
    if (name == "is_command_panel") return TILE_IS_COMMAND_PANEL;
    return TILE_NONE;
}

void AssetManagement::LoadTileProperties(uint32_t** out_buffer, int* out_count, Memory::Arena* arena, const char* tsj_path) {
    fstream stream(tsj_path);
    auto result = json::parse(stream);

    int tile_count = result["tilecount"].get<int>();
    uint32_t* buffer = ALLOC_ARRAY(arena, uint32_t, tile_count);

    if (result.contains("tiles")) {
        for (const auto& tile : result["tiles"]) {
            int tile_id = tile["id"].get<int>();
            if (!tile.contains("properties")) continue;
            for (const auto& prop : tile["properties"]) {
                string name = prop["name"].get<string>();
                TileFlag flag = FlagFromPropertyName(name);
                if (flag == TILE_NONE) continue;
                bool value = prop["value"].get<bool>();
                if (value) {
                    buffer[tile_id] = (uint32_t)(buffer[tile_id] | flag);
                }
            }
        }
    }

    *out_buffer = buffer;
    *out_count = tile_count;
}

bool TileHasFlag(uint32_t* tileFlags, int local_tile_id, TileFlag flag) {
    if (local_tile_id < 0) {
        return false;
    }
    return (tileFlags[local_tile_id] & flag) == flag;
}