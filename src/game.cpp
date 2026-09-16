#include "game.h"
#include "image.h"
#include <SDL3/SDL_log.h>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_render.h>
#include <algorithm>
#include <cstdlib>
#include "levelRenderer.h"
#include "common.h"
#include "dev_gui.h"
#include "hackUi.h"
#include "imgui/imgui.h"
#include "input.h"

using namespace Sokoban;


bool TryMove(Entity* mover, LevelData* level, CommandBuffer* cmd_buffer, int xDir, int yDir, int timestamp)
{
    int test_x = mover->x + xDir;
    int test_y = mover->y + yDir;

    Entity* stack[8];
    int count = level->GetEntitiesAt(test_x, test_y, stack, 8);

    Entity* blocker = nullptr;
    bool hasWalkThrough = false;

    for (int i = 0; i < count; i++) {
        if (stack[i]->HasBehaviour(CAN_MOVE)) {
            blocker = stack[i];
        }
        else if (stack[i]->HasBehaviour(CAN_WALK_THROUGH)) {
            hasWalkThrough = true;
        }
    }

    if (blocker != nullptr) {
        if (TryMove(blocker, level, cmd_buffer, xDir, yDir, timestamp)) {
            MoveCommand mv;
            mv.type = CMD_TYPE::MOVE;
            mv.entity = mover;
            mv.xDir = xDir;
            mv.yDir = yDir;
            Push(cmd_buffer, mv, timestamp);
            return true;
        }
        return false;
    }

    if (count == 0) {
        uint8_t stepInto_tile_id = level->GetCellID(test_x, test_y);
        if (
            stepInto_tile_id == (uint8_t)ID::GRASS ||
            stepInto_tile_id == (uint8_t)ID::GRASS_SHADOW ||
            stepInto_tile_id == (uint8_t)ID::COMMAND_PANEL ||
            stepInto_tile_id == (uint8_t)ID::EXIT ||
            stepInto_tile_id == (uint8_t)ID::YELLOW_BUTTON ||
            stepInto_tile_id == (uint8_t)ID::YELLOW_BUTTON_DOWN ||
            stepInto_tile_id == (uint8_t)ID::RED_BUTTON ||
            stepInto_tile_id == (uint8_t)ID::RED_BUTTON_DOWN ||
            stepInto_tile_id == (uint8_t)ID::SPIKE ||
            stepInto_tile_id == (uint8_t)ID::SPIKE_DOWN
            ) {
            MoveCommand mv;
            mv.type = CMD_TYPE::MOVE;
            mv.entity = mover;
            mv.xDir = xDir;
            mv.yDir = yDir;
            Push(cmd_buffer, mv, timestamp);
            return true;
        }
        return false;
    }

    if (hasWalkThrough) {
        MoveCommand mv;
        mv.type = CMD_TYPE::MOVE;
        mv.entity = mover;
        mv.xDir = xDir;
        mv.yDir = yDir;
        Push(cmd_buffer, mv, timestamp);
        return true;
    }

    return false;
}

extern "C"
{
    void Initialize(GameData* data, SDL_Window* window, SDL_Renderer* renderer)
    {
        AssetManagement::LoadAllSprites(data, renderer);
        data->currentLevel = 0;
        CreateLevel(data->arena_levels, &data->levels[0], "assets/levels/playTestArea.tmj");
        CreateDecorations(data->arena_levels, &data->levels[0], "assets/levels/playTestArea.tmj");        
        
        CreateLevel(data->arena_levels, &data->levels[1], "assets/levels/startMap.tmj");
        CreateDecorations(data->arena_levels, &data->levels[1], "assets/levels/startMap.tmj");

        CreateLevel(data->arena_levels, &data->levels[2], "assets/levels/lvl2.tmj");
        CreateDecorations(data->arena_levels, &data->levels[2], "assets/levels/lvl2.tmj");        
        


        CreateEntities(&data->levels[data->currentLevel], data->arena_entities);
        data->hackUiPanel.x = 50;
        data->hackUiPanel.y = 50;
        data->hackUiPanel.w = 800;
        data->hackUiPanel.h = 600;
        DEV::Initialize(window, renderer);
        data->imGui_context = ImGui::GetCurrentContext();
    }

    bool HandleEvents(GameData* data, SDL_Event event)
    {
        DEV::ProcessEvents(&event);
        if (event.type == SDL_EVENT_QUIT)
        {
            return false;
        }
        return true;
    }

    void Update(GameData* data, float dt) {
        const bool* keys = SDL_GetKeyboardState(nullptr);

        if (KeyPressed(&data->input, SDL_SCANCODE_Z) || KeyHeld_ForTime(&data->input,
            SDL_SCANCODE_Z, UNDO_REPEAT_TIME)) {
                ResetKeyHeldTime(&data->input, SDL_SCANCODE_Z);
            if (KeyHeld(&data->input, SDL_SCANCODE_LSHIFT)) {
                Redo(data->commandBuffer);
            }
            else {
                Undo(data->commandBuffer);
            }
        }


        if (KeyPressed(&data->input, SDL_SCANCODE_SPACE)){
            for (int i = 0; i < data->GetCurrentLevel()->entityCount; i++) {
                Entity* entity = &data->GetCurrentLevel()->entityBuffer[i];
                if (entity->HasBehaviour(CAN_MOVE) && entity->HasBehaviour(RESPOND_TO_INPUT)) {
                    uint8_t cellID = data->GetCurrentLevel()->GetCellID(entity->x, entity->y);
                    if (cellID == (uint8_t)ID::COMMAND_PANEL) {
                        data->hackUiOpen = !data->hackUiOpen;
                    }
                }
            }
        }

        if (KeyPressed(&data->input, SDL_SCANCODE_RIGHT) ||
            KeyHeld_ForTime(&data->input, SDL_SCANCODE_RIGHT, (1 / MOVE_SPEED) * 1.15)) {
            
                ResetKeyHeldTime(&data->input, SDL_SCANCODE_RIGHT);
            data->input_buffer[data->input_buffer_write_count++ % data->input_buffer_capacity] =
            { 1, 0 }; 
        }
        else if (KeyPressed(&data->input, SDL_SCANCODE_LEFT) ||
            KeyHeld_ForTime(&data->input, SDL_SCANCODE_LEFT, (1 / MOVE_SPEED) * 1.15)) {
                ResetKeyHeldTime(&data->input, SDL_SCANCODE_LEFT);
            data->input_buffer[data->input_buffer_write_count++ % data->input_buffer_capacity] =
            { -1, 0 }; 
        }
        else if (KeyPressed(&data->input, SDL_SCANCODE_UP) ||
            KeyHeld_ForTime(&data->input, SDL_SCANCODE_UP, (1 / MOVE_SPEED) * 1.15)) {
                ResetKeyHeldTime(&data->input, SDL_SCANCODE_UP);
            data->input_buffer[data->input_buffer_write_count++ % data->input_buffer_capacity] =
            { 0,-1 }; 
        }
        else if (KeyPressed(&data->input, SDL_SCANCODE_DOWN) ||
            KeyHeld_ForTime(&data->input, SDL_SCANCODE_DOWN, (1 / MOVE_SPEED) * 1.15)) {
                ResetKeyHeldTime(&data->input, SDL_SCANCODE_DOWN);
            data->input_buffer[data->input_buffer_write_count++ % data->input_buffer_capacity] =
            { 0, 1 };
        }


        HACKUI::UpdateUIPanel(data,data->hackUiPanel);

		//Move moving entities
        bool are_entities_moving = false;
        for (int i = 0; i < data->GetCurrentLevel()->entityCount; i++) {
            Entity* entity = &data->GetCurrentLevel()->entityBuffer[i];
            if (entity->HasBehaviour(CAN_MOVE) && IsMoving(entity)) {
                entity->progress_01 += MOVE_SPEED * dt;
                if (entity->progress_01 >= 1) {
                    entity->progress_01 = 0;
                    entity->x_prev = entity->x;
                    entity->y_prev = entity->y;
                }
                if (IsMoving(entity)) {
                    are_entities_moving = true;
                }

            }
        }
        if (are_entities_moving == false) {
            if (data->input_buffer_read_count == data->input_buffer_write_count) {
                return;
            }
            data->command_timestamp += 1;
            for (int i = 0; i < data->GetCurrentLevel()->entityCount; i++) {
                Entity* entity = &data->GetCurrentLevel()->entityBuffer[i];
                if (entity->HasBehaviour((Behaviour)(RESPOND_TO_INPUT | CAN_MOVE))) {
                    int xDir = data->input_buffer[data->input_buffer_read_count %  data->input_buffer_capacity].x;
                    int yDir = data->input_buffer[data->input_buffer_read_count % data->input_buffer_capacity].y;
                    TryMove(entity, data->GetCurrentLevel(), data->commandBuffer, xDir, yDir, data->command_timestamp);
                }
            }
  


            data->input_buffer_read_count++;
        }

    }
    

    void Draw(GameData* data, SDL_Renderer* renderer)
    {
        DEV::PreDraw(data->imGui_context);

        // 2. Clear
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // 3. Draw your game
        RenderLevel(data, renderer);
        RenderEntities(data, renderer);
        RenderDecorations(data, renderer);

        // 4. Draw ImGui on top of everything
        if (data->hackUiOpen) {
	
            HACKUI::DrawUIPanel(data->hackUiPanel, renderer);
        }
        DEV::Draw(data, renderer);

        
        // 5. Present (must be last)
        SDL_RenderPresent(renderer);
    }


    void OnQuit(SDL_Renderer* renderer)
    {
        SDL_DestroyRenderer(renderer);
    }
}