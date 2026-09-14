#include "imgui/imgui.h"
#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"
#include "hackUi.h"
#include "command.h"
#include "gameState.h"
using namespace std;

    bool IsInRect(SDL_FRect rect, float x, float y) {
        bool IsInRect = x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;

        return IsInRect;
    }

    void HACKUI::UpdateUIPanel(Sokoban::GameData* data, UIPanel& panel) {
        float mouseX;
        float mouseY;

		SDL_GetMouseState(&mouseX, &mouseY);

        SDL_FRect header = { panel.x, panel.y, panel.w, 20 };

        if (data->hackUiOpen && IsInRect(header, mouseX, mouseY)) {
            
        }
    }
    void HACKUI::DrawUIPanel(UIPanel& panel, SDL_Renderer* renderer) {

        
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
		SDL_FRect window = { panel.x, panel.y, panel.w, panel.h };
        SDL_RenderFillRect(renderer, &window);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_FRect border = { panel.x, panel.y, panel.w, panel.h };
        SDL_FRect header = { panel.x, panel.y, panel.w, 20 };
        SDL_RenderRect(renderer, &border);
        SDL_RenderFillRect(renderer, &header);
    }



