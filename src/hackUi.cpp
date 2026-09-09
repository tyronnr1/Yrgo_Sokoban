#include "imgui/imgui.h"
#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"
#include "hackUi.h"
#include "command.h"

using namespace std;

    void HACKUI::DrawUIPanel(UIPanel& panel) {


        ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_Always);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 100));
        ImGui::PushStyleColor(ImGuiCol_TitleBg, IM_COL32(0, 100, 0, 255));
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, IM_COL32(0, 150, 0, 255));

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize;

        ImGui::Begin("The Main Frame", nullptr, flags);


        ImGui::End();
        ImGui::PopStyleColor(3);

    }
