#include "game.h"
#include "image.h"
#include <SDL3/SDL_log.h>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_render.h>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include "levelRenderer.h"
#include "common.h"
#include "dev_gui.h"
#include "hackUi.h"
#include "imgui/imgui.h"
#include "input.h"
#include "leveleditor.h"
#include "rendering.h"

using namespace Sokoban;

bool TryMove(Entity* mover, LevelData* level, CommandBuffer* cmd_buffer, int xDir, int yDir, int strength)
{
    if (strength < 0) {
        return false;
    }

    int test_x = mover->x + xDir;
    int test_y = mover->y + yDir;

    Entity* stack[8];
    int count = level->GetEntitiesAt(test_x, test_y, stack, 8);

    Entity* blocker = nullptr;
    bool hasWalkThrough = false;

    for (int i = 0; i < count; i++) {
        if (HasBehaviour(stack[i], CAN_MOVE)) {
            blocker = stack[i];
        }
        else if (HasBehaviour(stack[i], CAN_WALK_THROUGH)) {
            hasWalkThrough = true;
        }
    }

    if (blocker != nullptr) {
        if ((HasBehaviour(blocker, IS_HEAVY) && !HasBehaviour(mover, IS_PLAYER)) || HasBehaviour(mover, IS_HEAVY)) {
            return false;
        }

        if (TryMove(blocker, level, cmd_buffer, xDir, yDir, --strength)) {
            MoveCommand mv;
            mv.type = CMD_TYPE::MOVE;
            mv.entity = mover;
            mv.xDir = xDir;
            mv.yDir = yDir;
            AddBehaviour(mover, IS_PUSHING);
            Push(cmd_buffer, mv, level);
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
            Push(cmd_buffer, mv, level);
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
        Push(cmd_buffer, mv, level);
        return true;
    }

    return false;
}

void InitializeGame(Gameplay* gameplay, Memory::Arena* arena_levels, Memory::Arena* arena_entities) {
    assert(gameplay->initialized == false);
    gameplay->currentLevel = 0;

    CreateLevel(arena_levels, &gameplay->levels[0], "assets/levels/playTestArea.tmj");
    CreateDecorations(arena_levels, &gameplay->levels[0], "assets/levels/playTestArea.tmj");

    CreateLevel(arena_levels, &gameplay->levels[1], "assets/levels/startMap.tmj");
    CreateDecorations(arena_levels, &gameplay->levels[1], "assets/levels/startMap.tmj");

    CreateLevel(arena_levels, &gameplay->levels[2], "assets/levels/lvl2.tmj");
    CreateDecorations(arena_levels, &gameplay->levels[2], "assets/levels/lvl2.tmj");

    CreateEntities(&gameplay->levels[gameplay->currentLevel], arena_entities);

    gameplay->hackUiPanel.x = 50;
    gameplay->hackUiPanel.y = 50;
    gameplay->hackUiPanel.w = 800;
    gameplay->hackUiPanel.h = 600;

    gameplay->initialized = true;
}

void StartLevel(Gameplay* gameplay, Memory::Arena* arena_commands, Memory::Arena* arena_entities) {
    Memory::Reset(arena_commands);
    CreateEntities(&gameplay->levels[gameplay->currentLevel], arena_entities);
}

void ChangeScene(GameData* data, SCENE_TYPES new_scene) {
    assert(new_scene != data->scene_current);
    data->scene_previous = data->scene_current;
    data->scene_current = new_scene;
    data->transition.state = data->scene_previous == SCENE_TYPES::NONE ? Transition::FadeFrom : Transition::FadeTo;
    data->transition.fade_time_elapsed = 0;

    switch (data->scene_current) {
    case SCENE_TYPES::TITLESCREEN:
        data->transition.fade_time_duration = 1;
        break;
    case SCENE_TYPES::MAINMENU:
        break;
    case SCENE_TYPES::GAME: {
        data->transition.fade_time_duration = 0.5f;
        Gameplay* gameplay = &data->scenes.gameplay;
        assert(gameplay->initialized);
        StartLevel(gameplay, data->arena_commands, data->arena_entities);
        break;
    }
    case SCENE_TYPES::CREDITS:
        break;
    case SCENE_TYPES::NONE:
        assert(false);
        break;
    }
}

void UpdateTitlescreen(TitleScreen* titlescreen, const float dt) {
    // nothing yet — add menu/animation logic here later
}

void UpdateGame(GameData* data, Gameplay* gameplay, Input* input, const float dt) {
    if (KeyPressed(input, SDL_SCANCODE_Z) || KeyHeld_ForTime(input, SDL_SCANCODE_Z, UNDO_REPEAT_TIME)) {
        ResetKeyHeldTime(input, SDL_SCANCODE_Z);
        if (KeyHeld(input, SDL_SCANCODE_LSHIFT)) {
            Redo(gameplay->commandBuffer, gameplay->GetCurrentLevel());
        }
        else {
            Undo(gameplay->commandBuffer, gameplay->GetCurrentLevel());
        }
    }

    if (KeyPressed(input, SDL_SCANCODE_SPACE)) {
        for (int i = 0; i < gameplay->GetCurrentLevel()->entityCount; i++) {
            Entity* entity = &gameplay->GetCurrentLevel()->entityBuffer[i];
            if (HasBehaviour(entity, CAN_MOVE) && HasBehaviour(entity, RESPOND_TO_INPUT)) {
                uint8_t cellID = gameplay->GetCurrentLevel()->GetCellID(entity->x, entity->y);
                if (cellID == (uint8_t)ID::COMMAND_PANEL) {
                    gameplay->hackUiOpen = !gameplay->hackUiOpen;
                    if (gameplay->hackUiOpen) {
                        AddBehaviour(entity, IS_HACKING);
                    }
                    else {
                        RemoveBehaviour(entity, IS_HACKING);
                    }
                }
            }
        }
    }

    if (KeyPressed(input, SDL_SCANCODE_RIGHT) ||
        KeyHeld_ForTime(input, SDL_SCANCODE_RIGHT, (1 / MOVE_SPEED) * 1.15)) {
        ResetKeyHeldTime(input, SDL_SCANCODE_RIGHT);
        gameplay->input_buffer[gameplay->input_buffer_write_count++ % gameplay->input_buffer_capacity] = { 1, 0 };
    }
    else if (KeyPressed(input, SDL_SCANCODE_LEFT) ||
        KeyHeld_ForTime(input, SDL_SCANCODE_LEFT, (1 / MOVE_SPEED) * 1.15)) {
        ResetKeyHeldTime(input, SDL_SCANCODE_LEFT);
        gameplay->input_buffer[gameplay->input_buffer_write_count++ % gameplay->input_buffer_capacity] = { -1, 0 };
    }
    else if (KeyPressed(input, SDL_SCANCODE_UP) ||
        KeyHeld_ForTime(input, SDL_SCANCODE_UP, (1 / MOVE_SPEED) * 1.15)) {
        ResetKeyHeldTime(input, SDL_SCANCODE_UP);
        gameplay->input_buffer[gameplay->input_buffer_write_count++ % gameplay->input_buffer_capacity] = { 0,-1 };
    }
    else if (KeyPressed(input, SDL_SCANCODE_DOWN) ||
        KeyHeld_ForTime(input, SDL_SCANCODE_DOWN, (1 / MOVE_SPEED) * 1.15)) {
        ResetKeyHeldTime(input, SDL_SCANCODE_DOWN);
        gameplay->input_buffer[gameplay->input_buffer_write_count++ % gameplay->input_buffer_capacity] = { 0, 1 };
    }

    HACKUI::UpdateUIPanel(data, gameplay->hackUiPanel);

    bool are_entities_moving = false;
    for (int i = 0; i < gameplay->GetCurrentLevel()->entityCount; i++) {
        Entity* entity = &gameplay->GetCurrentLevel()->entityBuffer[i];

        if (HasBehaviour(entity, CAN_MOVE) && IsMoving(entity)) {
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
        if (gameplay->input_buffer_read_count == gameplay->input_buffer_write_count) {
            return;
        }
        gameplay->commandBuffer->command_timestamp += 1;
        for (int i = 0; i < gameplay->GetCurrentLevel()->entityCount; i++) {
            Entity* entity = &gameplay->GetCurrentLevel()->entityBuffer[i];

            if (HasBehaviour(entity, IS_PUSHING)) {
                RemoveBehaviour(entity, IS_PUSHING);
            }

            if (HasBehaviour(entity, (Behaviour)(RESPOND_TO_INPUT | CAN_MOVE))) {
                int xDir = gameplay->input_buffer[gameplay->input_buffer_read_count % gameplay->input_buffer_capacity].x;
                int yDir = gameplay->input_buffer[gameplay->input_buffer_read_count % gameplay->input_buffer_capacity].y;

                Direction new_facing = DirectionFromXY(xDir, yDir);
                if (new_facing != entity->facing) {
                    RotateCommand rotate(entity, entity->facing, new_facing);
                    Push(gameplay->commandBuffer, rotate, gameplay->GetCurrentLevel());
                }

                TryMove(entity, gameplay->GetCurrentLevel(), gameplay->commandBuffer, xDir, yDir, entity->strength);
            }
        }
        gameplay->input_buffer_read_count++;
    }
}

void RenderDebugTextScaled(SDL_Renderer* renderer, float x, float y, const char* text, float scale) {
    SDL_SetRenderScale(renderer, scale, scale);
    SDL_RenderDebugText(renderer, x / scale, y / scale, text);
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);
}

void DrawScene(GameData* data, SCENE_TYPES scene, SDL_Renderer* renderer) {
    switch (scene) {
    case SCENE_TYPES::TITLESCREEN: {
        RenderSprite_Fullscreen(data->titleScreenArt, renderer, data->screenW, data->screenH);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        RenderDebugTextScaled(renderer, data->screenW / 2.0f - 140, data->screenH / 2.0f - 30, "SOKOBAN", 4.0f);
        RenderDebugTextScaled(renderer, data->screenW / 2.0f - 170, data->screenH / 2.0f + 50, "Press any key to start", 2.0f);
        break;
    }
    case SCENE_TYPES::MAINMENU:
    case SCENE_TYPES::GAME:
        RenderLevel(data, renderer);
        RenderEntities(data, renderer);
        RenderDecorations(data, renderer);
        if (data->scenes.gameplay.hackUiOpen) {
            HACKUI::DrawUIPanel(data->scenes.gameplay.hackUiPanel, renderer);
        }
        break;
    case SCENE_TYPES::CREDITS:
        break;
    case SCENE_TYPES::NONE:
        assert(false);
        break;
    }
}

void RenderFullscreenFade(SDL_Renderer* renderer, int screenW, int screenH, float alpha) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, (Uint8)(SDL_clamp(alpha, 0.0f, 1.0f) * 255));
    SDL_FRect full = { 0, 0, (float)screenW, (float)screenH };
    SDL_RenderFillRect(renderer, &full);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
extern "C"
{
    void Initialize(GameData* data, SDL_Window* window, SDL_Renderer* renderer)
    {
        DEV::Initialize(window, renderer);
        AssetManagement::LoadAllSprites(data, renderer);
        data->imGui_context = ImGui::GetCurrentContext();

        InitializeGame(&data->scenes.gameplay, data->arena_levels, data->arena_entities);
        ChangeScene(data, SCENE_TYPES::TITLESCREEN);
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
        Gameplay* gameplay = &data->scenes.gameplay;
        TitleScreen* titlescreen = &data->scenes.titlescreen;
        Transition* transition = &data->transition;

        if (KeyPressed(&data->input, SDL_SCANCODE_F2)) {
            data->edit_level = !data->edit_level;
        }
        if (data->edit_level) {
            EDITOR::Update(&data->editorData, &data->input, gameplay->GetCurrentLevel(), gameplay->commandBuffer, data->screenW, data->screenH);
        }

        if (KeyPressed(&data->input, SDL_SCANCODE_5) && data->scene_current != SCENE_TYPES::TITLESCREEN) {
            ChangeScene(data, SCENE_TYPES::TITLESCREEN);
            return;
        }

        if (transition->state != Transition::Inactive) {
            transition->fade_time_elapsed += dt;
            if (transition->fade_time_elapsed >= transition->fade_time_duration) {
                transition->fade_time_elapsed = 0;
                switch (transition->state) {
                case Transition::Inactive:
                    break;
                case Transition::FadeTo:
                    transition->state = Transition::FadeFrom;
                    break;
                case Transition::FadeFrom:
                    transition->state = Transition::Inactive;
                    break;
                }
            }
        }

        switch (data->scene_current) {
        case SCENE_TYPES::TITLESCREEN:
            UpdateTitlescreen(titlescreen, dt);
            if (AnyKeyPressed(&data->input)) {
                if (transition->state == Transition::FadeTo || transition->state == Transition::Inactive) {
                    ChangeScene(data, SCENE_TYPES::GAME);
                }
            }
            break;
        case SCENE_TYPES::MAINMENU:
            break;
        case SCENE_TYPES::GAME:
            UpdateGame(data, gameplay, &data->input, dt);
            break;
        case SCENE_TYPES::CREDITS:
            break;
        case SCENE_TYPES::NONE:
            assert(false);
            break;
        }
    }

    void Draw(GameData* data, SDL_Renderer* renderer) {
        DEV::PreDraw(data->imGui_context);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        switch (data->transition.state) {
        case Transition::Inactive:
            DrawScene(data, data->scene_current, renderer);
            break;
        case Transition::FadeTo: {
            DrawScene(data, data->scene_previous, renderer);
            float alpha = data->transition.fade_time_elapsed / data->transition.fade_time_duration;
            RenderFullscreenFade(renderer, data->screenW, data->screenH, alpha);
            break;
        }
        case Transition::FadeFrom: {
            DrawScene(data, data->scene_current, renderer);
            float alpha = 1 - data->transition.fade_time_elapsed / data->transition.fade_time_duration;
            RenderFullscreenFade(renderer, data->screenW, data->screenH, alpha);
            break;
        }
        }

        DEV::Draw(data, renderer);
        SDL_RenderPresent(renderer);
    }

    void OnQuit(SDL_Renderer* renderer)
    {
        SDL_DestroyRenderer(renderer);
    }
}