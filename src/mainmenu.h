#pragma once

struct SDL_Renderer;
struct Image;
namespace Memory { struct Arena; }
namespace Sokoban { struct GameData; struct MainMenu; }

void InitializeMenu(Sokoban::MainMenu* mainmenu, Image* fallback, Memory::Arena* arena_main, int screenW, int screenH);
void UpdateMenu(Sokoban::GameData* data);
void DrawMenu(Sokoban::GameData* data, Sokoban::MainMenu* mainmenu, SDL_Renderer* renderer);