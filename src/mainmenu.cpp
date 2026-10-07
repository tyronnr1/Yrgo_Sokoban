#include "mainmenu.h"
#include "SDL3/SDL_scancode.h"
#include "arena.h"
#include "button.h"
#include "gameState.h"
#include "input.h"
#include "rendering.h"
#include "image.h"
#include <cassert>

using namespace Sokoban;

void InitializeMenu(MainMenu* mainmenu, Image* fallback, Memory::Arena* arena_main, int screenW, int screenH) {
    assert(mainmenu->initialized == false);
    mainmenu->button_count = 2;
    mainmenu->buttons = ALLOC_ARRAY(arena_main, Button, mainmenu->button_count);

    SetupButton(&mainmenu->buttons[0], ButtonType::START_GAME, fallback,
        { screenW / 2.0f, screenH / 2.0f, 200, 80 }, ButtonMode::Centered);
    SetupButton(&mainmenu->buttons[1], ButtonType::QUIT, fallback,
        { screenW / 2.0f, (screenH / 2.0f) + 100, 200, 80 }, ButtonMode::Centered);

    mainmenu->initialized = true;
}

void DrawMenu(GameData* data, MainMenu* mainmenu, SDL_Renderer* renderer) {
    RenderSprite_Fullscreen(data->titleScreenArt, renderer, data->screenW, data->screenH);
    for (int i = 0; i < mainmenu->activeButtonCount; i++) {
        RenderButton(mainmenu->activeButtons[i], i == mainmenu->activeButtonIndex, renderer);
    }
}

void UpdateMenu(GameData* data) {
    MainMenu* mainmenu = &data->scenes.mainMenu;
    Input* input = &data->input;

    mainmenu->activeButtonCount = GetActiveButtonCount(mainmenu->buttons, mainmenu->button_count);
    if (mainmenu->activeButtonCount == 0) {
        return;
    }
    mainmenu->activeButtons = ALLOC_ARRAY(data->arena_scratch, Button*, mainmenu->activeButtonCount);

    int index = 0;
    for (int i = 0; i < mainmenu->button_count; i++) {
        Button* button = &mainmenu->buttons[i];
        if (button->is_active) {
            mainmenu->activeButtons[index] = button;
            index += 1;
        }
    }

    int* buttonIndex = &mainmenu->activeButtonIndex;

    bool mouseMoving = input->mouse_magnitude > 0.1;
    if (mouseMoving) {
        for (int i = 0; i < mainmenu->activeButtonCount; i++) {
            Button* button = mainmenu->activeButtons[i];
            if (IsHoveredOver(button, input->mouse_x, input->mouse_y)) {
                *buttonIndex = i;
                break;
            }
        }
    }

    bool up = KeyPressed(input, SDL_SCANCODE_UP);
    bool down = KeyPressed(input, SDL_SCANCODE_DOWN);

    if (up || down) {
        int direction = up ? 1 : -1;
        *buttonIndex += direction + mainmenu->activeButtonCount;
        *buttonIndex = *buttonIndex % mainmenu->activeButtonCount;
    }

    Button* selected = mainmenu->activeButtons[*buttonIndex];

    if (selected != nullptr) {
        if (KeyPressed(input, SDL_SCANCODE_RETURN)) {
            PressButton(selected, data);
            return;
        }
        if (IsHoveredOver(selected, input->mouse_x, input->mouse_y)) {
            if (MousePressed(input, MouseButtons::LEFT)) {
                PressButton(selected, data);
                return;
            }
        }
    }
}