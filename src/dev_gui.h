#pragma once

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"

// Forward declarations
namespace Sokoban {
    struct GameData;
}
struct ImGuiContext;

namespace DEV {

    void Initialize(SDL_Window* window, SDL_Renderer* renderer);
    void ProcessEvents(SDL_Event* event);
    void PreDraw(ImGuiContext* saved_context);
    void Draw(Sokoban::GameData* data, SDL_Renderer* renderer);

}