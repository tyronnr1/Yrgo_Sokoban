#pragma once
#include "SDL3/SDL_rect.h"
#include "arena.h"
#include "levels.h"
#include "command.h"
#include "input.h"
#include "hackUi.h"
#include "camera.h"
#include "editor.h"

struct Image;
struct ImGuiContext;

namespace Sokoban
{

    struct Position {
        float x;
        float y;
    };

    enum class SCENE_TYPES : uint8_t {
        NONE,
        TITLESCREEN,
        MAINMENU,
        GAME,
        CREDITS,
    };

    struct Gameplay {
        CommandBuffer* commandBuffer = nullptr;
        LevelData* levels = nullptr;
        int levelCount = 0;
        int currentLevel = 0;

        Position* input_buffer = nullptr;
        int input_buffer_capacity = 0;
        int input_buffer_write_count = 0;
        int input_buffer_read_count = 0;

        bool initialized = false;

        bool hackUiOpen = false;
        UIPanel hackUiPanel;

        LevelData* GetCurrentLevel() {
            return &levels[currentLevel];
        }

        int activePlayerIndex;
        Entity** activePlayerBuffer;
    };

    struct TitleScreen {
        

    };

    struct MainMenu {
    };

    struct Credits {
    };

    struct Scenes {
        Gameplay gameplay;
        TitleScreen titlescreen;
        MainMenu mainMenu;
        Credits credits;
    };

    struct Transition {
        enum States {
            Inactive,
            FadeTo,
            FadeFrom
        };
        States state = Inactive;
        float fade_time_elapsed = 0.0f;
        float fade_time_duration = 1.0f;
    };

    struct EditorData {
        float* fps_buffer;
        int fps_buffer_count;
        int fps_buffer_index;
    };

    struct GameData
    {


        SCENE_TYPES scene_current = SCENE_TYPES::NONE;
        SCENE_TYPES scene_previous = SCENE_TYPES::NONE;
        Scenes scenes;
        Transition transition;

        // --- Everything that is shared between scenes ---
        Camera camera;
        int screenW = 0;
        int screenH = 0;
        float currentFPS = 0.0f;

        Image* fallback = nullptr;
        Image* player = nullptr;
        Image* tileset = nullptr;
        Image* dropshadow = nullptr;
        Image* titleScreenArt = nullptr;
        Image* blackPixel = nullptr;
        Image* selectionMarker = nullptr;

        Memory::Arena* arena_levels = nullptr;
        Memory::Arena* arena_entities = nullptr;
        Memory::Arena* arena_images = nullptr;
        Memory::Arena* arena_commands = nullptr;
        Memory::Arena* arena_input = nullptr;
        Memory::Arena* arena_scratch = nullptr;

        Input input;
        float* dt = nullptr;
        float* dt_scaler = nullptr;
        ImGuiContext* imGui_context = nullptr;

        bool edit_level = false;
        Editor editorData;
		EditorData editor_data;
        uint32_t* tileFlags = nullptr;
        int tileFlagsCount = 0;
    };

}