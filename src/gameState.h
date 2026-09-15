#pragma once
#include "SDL3/SDL_rect.h"
#include "arena.h"
#include "levels.h"
#include "command.h"
#include "input.h"
#include "hackUi.h"
#include "camera.h"
struct Image;
struct ImGuiContext;

namespace Sokoban
{

    struct GameData
    {
        struct Position
        {
            float x;
            float y;
        };
        Camera camera;

        int screenW;
        int screenH;
        
        float currentFPS;

        Image* fallback;
        Image* player;
        Image* tileset;

        Memory::Arena* arena_levels;
        Memory::Arena* arena_entities;
        Memory::Arena* arena_images;
        Memory::Arena* arena_commands;

        CommandBuffer* commandBuffer;

        LevelData* levels;
        int levelCount;
        int currentLevel;
        LevelData* GetCurrentLevel(){ 
            return &levels[currentLevel];
        }

        Input input;
        Memory::Arena* arena_input;

        const float* dt = nullptr;
        ImGuiContext* imGui_context = nullptr;

        uint32_t command_timestamp;

        Position* input_buffer;
        int input_buffer_capacity;
        int input_buffer_write_count;
        int input_buffer_read_count;

        bool hackUiOpen = false;
        UIPanel hackUiPanel;
    };
}