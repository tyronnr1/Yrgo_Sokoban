#include "dev_gui.h"

#include "gameState.h"
#include "command.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"

#include <string>
#include <cmath>
#include "leveleditor.h"

using namespace std;


static void Draw_Imgui_Arena_Usage(Memory::Arena* arena, const string& name)
{
    float fraction = (float)arena->used / (float)arena->size;

    string barText = name;
    barText += " " + to_string(arena->used);
    barText += " / " + to_string(arena->size);

    ImGui::ProgressBar(fraction, ImVec2(-1.0f, 0.0f), barText.c_str());
}

static void Draw_History(CommandBuffer* buffer, LevelData* level)
{
    int sliderPos = buffer->index;

    if (ImGui::SliderInt("history", &sliderPos, 0, buffer->head))
    {
        while (buffer->index > sliderPos)
            Undo(buffer, level);

        while (buffer->index < sliderPos)
            Redo(buffer, level);
    }
}

void DrawFPS(Sokoban::GameData* data) {
    Sokoban::EditorData* editor = &data->editor_data;

    editor->fps_buffer[editor->fps_buffer_index++] = 1.0 / *data->dt * *data->dt_scaler;
    editor->fps_buffer_index %= editor->fps_buffer_count;
    ImGui::PlotHistogram("fps", editor->fps_buffer, editor->fps_buffer_count, 0, nullptr , 0, data->currentFPS, ImVec2(-1, 35));
}



void DEV::Initialize(SDL_Window* window, SDL_Renderer* renderer)
{
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    ImGuiIO& io = ImGui::GetIO();
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    io.DisplaySize = ImVec2((float)w, (float)h);
}

void DEV::ProcessEvents(SDL_Event* event)
{
    ImGui_ImplSDL3_ProcessEvent(event);
}

void DEV::PreDraw(ImGuiContext* saved_context)
{
    if (ImGui::GetCurrentContext() == nullptr)
    {
        ImGui::SetCurrentContext(saved_context);
    }

    ImGui::NewFrame();
}

void DEV::Draw(Sokoban::GameData* data, SDL_Renderer* renderer)
{
    ImGui::Begin("Dev Tools");

    ImGui::Text("memory arena usage");

    Draw_Imgui_Arena_Usage(data->arena_images, "images");
    Draw_Imgui_Arena_Usage(data->arena_levels, "levels");
    Draw_Imgui_Arena_Usage(data->arena_commands, "commands");
    Draw_Imgui_Arena_Usage(data->arena_entities, "entities");

    Draw_History(data->scenes.gameplay.commandBuffer, data->scenes.gameplay.GetCurrentLevel());
    Draw_Imgui_Arena_Usage(data->arena_scratch, "scratch");

    if (data->dt)
    DrawFPS(data);

    ImGui::SliderFloat("deltaTimeScaler", data->dt_scaler, 0.1, 3);
    ImGui::End();
    
    if (data->edit_level) {
        EDITOR::DrawObjectPanel(&data->editorData, data->tileset, data->player);
        EDITOR::DrawPreview(&data->editorData, &data->input, renderer, data->scenes.gameplay.GetCurrentLevel(), &data->camera, data->tileset, data->player, data->screenW, data->screenH);
    }
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}