#include "imgui/imgui.h"
#include <SDL3/SDL.h>
#include <string>

#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"
#include "hackUi.h"
#include "command.h"
#include "gameState.h"
#include <fstream>

using namespace std;

    bool IsInRect(SDL_FRect rect, float x, float y) {
        bool IsInRect = x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;

        return IsInRect;
    }
    namespace HACKUI {
        Vector mouseStartPos;
        Vector mouseCurrentPos;
    }
    void HACKUI::UpdateUIPanel(Sokoban::GameData* data, UIPanel& panel) {

        SDL_FRect header = {
            panel.x,
            panel.y,
            panel.w,
            20
        };

        Vector mousePos = GetMousePosition(&data->input);

        if (!data->hackUiOpen)
            return;

        if (!panel.dragging && MousePressed(&data->input, MouseButtons::LEFT) &&
            IsInRect(header, mousePos.x, mousePos.y)) {

            panel.dragging = true;
            HACKUI::mouseStartPos = mousePos;
        }

        if (panel.dragging && MouseHeld(&data->input, MouseButtons::LEFT)) {

            Vector mouseDelta = {
                mousePos.x - HACKUI::mouseStartPos.x,
                mousePos.y - HACKUI::mouseStartPos.y
            };

            MovePanel(data, &panel, mouseDelta.x, mouseDelta.y);

            HACKUI::mouseStartPos = mousePos;
        }

        if (panel.dragging && MouseReleased(&data->input, MouseButtons::LEFT)) {
            panel.dragging = false;
        }
    }

    void HACKUI::DrawUIPanel(UIPanel& panel, SDL_Renderer* renderer) {

        
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
		SDL_FRect window = { panel.x, panel.y, panel.w, panel.h };
        SDL_RenderFillRect(renderer, &window);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        SDL_FRect border = { panel.x, panel.y, panel.w, panel.h };
        SDL_RenderRect(renderer, &border);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_FRect header = { panel.x, panel.y, panel.w, 20 };
        SDL_RenderFillRect(renderer, &header);
    }

    void HACKUI::MovePanel(Sokoban::GameData* data ,UIPanel* panel, float dx, float dy) {
        panel->x = SDL_clamp(panel->x + dx, 0.0f, (float)data->screenW - panel->w);
        panel->y = SDL_clamp(panel->y + dy, 0.0f, (float)data->screenH - panel->h);
	}