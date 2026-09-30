#pragma once

#include "camera.h"
#include "input.h"
#include "image.h"
#include "entity.h"
#include "levels.h"
#include "editor.h"
#include "command.h"

struct EditorObject {
    ID id;
    const char* label;
    bool is_tile;
};

namespace EDITOR {
    void DrawObjectPanel(Editor* editor, Image* tileset, Image* player);
    void PlaceObject(int x, int y, Editor* editor, LevelData* level, CommandBuffer* commandBuffer);
    void Update(Editor* editor, Input* input, LevelData* level, CommandBuffer* commandBuffer, int screenW, int screenH);
    void DrawPreview(Editor* editor, Input* input, SDL_Renderer* renderer, LevelData* level, Camera* camera, Image* tileset, Image* player, int screenW, int screenH);
}