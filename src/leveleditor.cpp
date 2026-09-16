#include "leveleditor.h"
#include "imgui/imgui.h"
#include "rendering.h"
#include "levelRenderer.h"

namespace EDITOR {

    static const EditorObject all_editor_objects[] = {
        { ID::WALL,        "Wall",        true  },
        { ID::GRASS,       "Grass",       true  },
        { ID::EXIT,        "Exit",        true  },
        { ID::RUSTY_WALL,  "Rusty Wall",  true  },
        { ID::YELLOW_BUTTON,"Yellow Btn", true  },
        { ID::RED_BUTTON,  "Red Btn",     true  },
        { ID::WATER,       "Water",       true  },
        { ID::SPIKE,       "Spike",       false },
        { ID::BOX_1,       "Box 1",       false },
        { ID::BOX_2,       "Box 2",       false },
        { ID::BOX_GREEN,   "Box Green",   false },
        { ID::BOX_METAL,   "Box Metal",   false },
        { ID::PLAYER,      "Player",      false },
    };

    void DrawObjectPanel(Editor* editor, Image* tileset, Image* player){
    ImGui::Begin("objects");
        ImVec2 size = { 32, 32 };

        for (const EditorObject& obj : all_editor_objects) {
            SDL_FRect src = GetTilesetSrcRect((int)obj.id, TILESET_FIRSTGID, TILESET_COLUMNS, TILESET_TILE_PX);
            ImVec2 uv0 = { src.x / tileset->width, src.y / tileset->height };
            ImVec2 uv1 = { (src.x + src.w) / tileset->width, (src.y + src.h) / tileset->height };

            if (obj.id == ID::PLAYER) {
                // player has its own dedicated texture, so just show the full image
                uv0 = { 0, 0 };
                uv1 = { 1, 1 };
            }

            Image* img = (obj.id == ID::PLAYER) ? player : tileset;
            if (ImGui::ImageButton(obj.label, (ImTextureID)img->texture, size, uv0, uv1)) {
                editor->object_to_place_id = obj.id;
            }
            ImGui::SameLine();
        }

        ImGui::End();
    }

    void PlaceObject(int x, int y, Editor* editor, LevelData* level) {
        for (const EditorObject& obj : all_editor_objects) {
            if (obj.id == editor->object_to_place_id) {
                if (obj.is_tile) {
                    level->cells[y * level->w + x] = (int)obj.id;
                }
                else {
                    AddEntity(obj.id, x, y, level);
                }
                return;
            }
        }
    }

    void Update(Editor* editor, Input* input, LevelData* level, int screenW, int screenH) {
        if (MousePressed(input, MouseButtons::LEFT)) {
            if (camera::GetIsPointInsideGrid(input->mouse_x, input->mouse_y, level, screenW, screenH)) {
                int x, y;
                camera::WorldToGrid(input->mouse_x, input->mouse_y, &x, &y, level, screenW, screenH);
                PlaceObject(x, y, editor, level);
            }
        }
        else if (MousePressed(input, MouseButtons::RIGHT)) {
            if (camera::GetIsPointInsideGrid(input->mouse_x, input->mouse_y, level, screenW, screenH)) {
                int x, y;
                camera::WorldToGrid(input->mouse_x, input->mouse_y, &x, &y, level, screenW, screenH);
                RemoveEntity(x, y, level);
            }
        }
    }

    void DrawPreview(Editor* editor, Input* input, SDL_Renderer* renderer, LevelData* level, Camera* camera, Image* tileset, Image* player, int screenW, int screenH) {
        int x, y;
        camera::WorldToGrid(input->mouse_x, input->mouse_y, &x, &y, level, screenW, screenH);

        if (editor->object_to_place_id == ID::PLAYER) {
            RenderSprite_Grid(player, level, renderer, camera, (float)x, (float)y, screenW, screenH, 1);
        }
        else {
            SDL_FRect srcRect = GetTilesetSrcRect((int)editor->object_to_place_id, TILESET_FIRSTGID, TILESET_COLUMNS, TILESET_TILE_PX);
            RenderSprite_Grid(tileset, level, renderer, camera, (float)x, (float)y, srcRect, screenW, screenH, 1);
        }
    }

}