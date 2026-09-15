#include <print>
#include <iostream>
#include <windows.h>

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"
#include "common.h"
#include "arena.h"
#include "gameState.h"
#include "image.h"
#include "gameState.h"
#include <timeapi.h>
#include <fstream>
#include <print>

using namespace std;

// ---- DLL hot-reload plumbing ----

using Sokoban::GameData;

typedef void (*Function_Initialize)(GameData* data, SDL_Window* window, SDL_Renderer* renderer); 
typedef bool (*Function_HandleEvents)(GameData* data, SDL_Event event);
typedef void (*Function_Update)(GameData* data, float dt);
typedef void (*Function_Draw)(GameData* data, SDL_Renderer* renderer);
typedef void (*Function_OnQuit)(SDL_Renderer* renderer);

constexpr const char* NAME_OF_DLL = "Sokoban_game.dll";
constexpr const char* NAME_OF_TEMP_DLL = "Sokoban_temp.dll";

constexpr const char* NAME_OF_FUNC_INIT = "Initialize";
constexpr const char* NAME_OF_FUNC_HANDLE_EVENT = "HandleEvents";
constexpr const char* NAME_OF_FUNC_UPDATE = "Update";
constexpr const char* NAME_OF_FUNC_DRAW = "Draw";
constexpr const char* NAME_OF_FUNC_QUIT = "OnQuit";

struct DLL_INFO
{
    HMODULE dll;
    FILETIME timestamp;
    Function_Initialize initialize;
    Function_HandleEvents handleEvents;
    Function_Update update;
    Function_Draw draw;
    Function_OnQuit quit;
};

FILETIME GetTimestamp()
{
    WIN32_FIND_DATA data;
    HANDLE handle = FindFirstFile(NAME_OF_DLL, &data);
    FILETIME time_of_last_change = data.ftLastWriteTime;
    FindClose(handle);
    return time_of_last_change;
}

bool LoadDLL(DLL_INFO* info, int depth = 0)
{
    if (depth > 20)
    {
        printf("failed to write temp DLL");
        return false;
    }

    bool success = CopyFile(NAME_OF_DLL, NAME_OF_TEMP_DLL, false);
    if (!success)
    {
        Sleep(50);
        return LoadDLL(info, depth + 1);
    }

    info->dll = LoadLibrary(NAME_OF_TEMP_DLL);
    if (info->dll == nullptr)
    {
        printf("could not load dll");
        return false;
    }

    info->initialize = (Function_Initialize)GetProcAddress(info->dll, NAME_OF_FUNC_INIT);
    info->handleEvents = (Function_HandleEvents)GetProcAddress(info->dll, NAME_OF_FUNC_HANDLE_EVENT);
    info->update = (Function_Update)GetProcAddress(info->dll, NAME_OF_FUNC_UPDATE);
    info->draw = (Function_Draw)GetProcAddress(info->dll, NAME_OF_FUNC_DRAW);
    info->quit = (Function_OnQuit)GetProcAddress(info->dll, NAME_OF_FUNC_QUIT);
    info->timestamp = GetTimestamp();

    return true;
}

void UnloadDLL(DLL_INFO* info)
{
    FreeLibrary(info->dll);
    info->dll = nullptr;
    DeleteFile(NAME_OF_TEMP_DLL);
}

void DLL_CheckStatus(DLL_INFO* dll)
{
    FILETIME timestamp = GetTimestamp();
    bool is_timestamp_changed = CompareFileTime(&dll->timestamp, &timestamp) != 0;
    if (is_timestamp_changed)
    {
        UnloadDLL(dll);
        LoadDLL(dll);
    }
}

void* AllocateGameMemory()
{
    void* blob = malloc(GAME_MEMORY_ALLOWANCE);

    if (blob == nullptr)
    {
        std::cerr << "Failed to allocate game memory!\n";
        exit(1);
    }

    return blob;
}

Uint64 NOW = 0;
Uint64 PREV = 0;

void CalculateDeltaTime(float& dt)
{
    NOW = SDL_GetTicksNS();
    dt = (float)(NOW - PREV);
    dt = SDL_NS_TO_SECONDS(dt);
    PREV = NOW;
}


void CalculateRemainingFrameTime_MS(double* milliseconds)
{
    Uint64 frame_end_time_ns = SDL_GetTicksNS();
    double frame_time_spent_ns = frame_end_time_ns - PREV;
    double frame_time_spent_ms = frame_time_spent_ns / 1e6;
    *milliseconds = FRAME_TIME_MS - frame_time_spent_ms;
}

void StoreGameState(Memory::Arena* arena)
{
    std::ofstream file("temp_state.bin", std::ios::binary);
    file.write(reinterpret_cast<const char*>(arena->memory), arena->size);
    file.close();
}

void RetrieveGameState(Memory::Arena* arena)
{
    std::ifstream file("temp_state.bin", std::ios::binary);
    file.read(reinterpret_cast<char*>(arena->memory), arena->size);
    file.close();
}

void RunSokoban()
{   
   
    void* game_memory = AllocateGameMemory();

    Memory::Arena* arena_main = new Memory::Arena();

    Memory::Initialize(
        arena_main,
        game_memory,
        GAME_MEMORY_ALLOWANCE
    );
    GameData* gameData = (GameData*)Memory::Allocate(
        arena_main,
        sizeof(GameData)
    );

    size_t IMAGE_ARENA_SIZE = sizeof(Image) * 1024;

    gameData->arena_images = Memory::CreateSubArena(arena_main, IMAGE_ARENA_SIZE);
    gameData->arena_levels = Memory::CreateSubArena(arena_main, MEGABYTES(3));
    gameData->arena_entities = Memory::CreateSubArena(arena_main, MEGABYTES(1));   
    
    gameData->input_buffer_capacity = 2;
    size_t RING_BUFFER_SIZE = sizeof(GameData::Position) * gameData->input_buffer_capacity;
    gameData->input_buffer = (GameData::Position*)Memory::Allocate(gameData->arena_levels, RING_BUFFER_SIZE);

    gameData->levelCount = 3;
    gameData->levels = (LevelData*)Memory::Allocate(arena_main, sizeof(LevelData) * gameData->levelCount);

    gameData->arena_commands = Memory::CreateSubArena(gameData->arena_levels, MEGABYTES(1));

    gameData->commandBuffer = (CommandBuffer*)Memory::Allocate(arena_main, sizeof(CommandBuffer));
    gameData->commandBuffer->capacity = 20000;
    size_t COMMAND_SIZE = sizeof(AnyCommand) * gameData->commandBuffer->capacity;
    gameData->commandBuffer->allCommands = (AnyCommand*)Memory::Allocate(gameData->arena_commands, COMMAND_SIZE);

    size_t INPUT_ARENA_SIZE = 0;

    INPUT_ARENA_SIZE += sizeof(bool) * SDL_SCANCODE_COUNT * 2;
    INPUT_ARENA_SIZE += sizeof(float) * SDL_SCANCODE_COUNT;
    INPUT_ARENA_SIZE += 128;

    int mouseButtonCount = 3;

    gameData->arena_input = Memory::CreateSubArena(arena_main, INPUT_ARENA_SIZE);

    gameData->input.keys_current = (bool*)Memory::Allocate(gameData->arena_input, sizeof(bool) * SDL_SCANCODE_COUNT);
    gameData->input.keys_previous = (bool*)Memory::Allocate(gameData->arena_input, sizeof(bool) * SDL_SCANCODE_COUNT);
    gameData->input.keys_held_time = (float*)Memory::Allocate(gameData->arena_input, sizeof(float) * SDL_SCANCODE_COUNT);
    gameData->input.mouse_held_time = (float*)Memory::Allocate(gameData->arena_input, sizeof(float) * mouseButtonCount);

    MMRESULT result = timeBeginPeriod(1);
    if (result == TIMERR_NOCANDO) {
        printf("could not increase timer resolution");
        Sleep(2000);
        return;
    }

    DLL_INFO dll;
    bool dll_successfully_loaded = LoadDLL(&dll);
    if (dll_successfully_loaded == false)
    {
        return;
    }

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    SDL_CreateWindowAndRenderer(
        "Sokoban",
        gameData->screenW,
        gameData->screenH,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED,
        &window,
        &renderer
    );    

    int actualW, actualH;
    SDL_GetWindowSize(window, &actualW, &actualH);
    gameData->screenW = actualW;
    gameData->screenH = actualH;

    dll.initialize(gameData, window, renderer);

    bool running = true;
    float dt;
    gameData->dt = &dt;

    while (running)
    {
        DLL_CheckStatus(&dll);
        CalculateDeltaTime(dt);
        gameData->currentFPS = (dt > 0.0f) ? (1.0f / dt) : 0.0f;
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            running = dll.handleEvents(gameData, event);
            if (running == false)
            {
                break;
            }
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_F9)
                {
                    StoreGameState(arena_main);
                }
                if (event.key.key == SDLK_F10)
                {
                    RetrieveGameState(arena_main);
                }
            }
        }
        gameData->input.keys_current = SDL_GetKeyboardState(nullptr);
        gameData->input.mouse_current = SDL_GetMouseState(&gameData->input.mouse_x,&gameData->input.mouse_y);
        dll.update(gameData, dt);
        UpdateKeys(&gameData->input, dt);

        dll.draw(gameData, renderer);
        UpdateMouse(&gameData->input, dt);

        double time_to_sleep_ms;
        CalculateRemainingFrameTime_MS(&time_to_sleep_ms);

        if (time_to_sleep_ms > 0) {
            if (time_to_sleep_ms > 1) {
                SDL_Delay(time_to_sleep_ms - 1);
            }
            while (time_to_sleep_ms > 0) {
                CalculateRemainingFrameTime_MS(&time_to_sleep_ms);
            }
        }
    }

    dll.quit(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


int main()
{
    RunSokoban();
    return 0;
}