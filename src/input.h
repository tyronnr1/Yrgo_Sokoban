#pragma once 
#include <SDL3/SDL_scancode.h>


namespace Sokoban {
	struct GameData;
}
struct Input {
	const bool* keys_current;
	const bool* keys_previous;
	float* keys_held_time;
};

bool KeyPressed(const Input* input, SDL_Scancode Key);
bool KeyHeld(const Input* input, SDL_Scancode Key);
bool KeyReleased(const Input* input, SDL_Scancode Key);
bool KeyHeld_ForTime(const Input* input, SDL_Scancode Key, float min_length);
void UpdateKeys(Input* input, float dt);
void ResetKeyHeldTime(Input* input, SDL_Scancode Key);
void ResetAll(Input* input);