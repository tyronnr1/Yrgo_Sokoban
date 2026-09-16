#pragma once

#include <string>
#include "input.h"
#include "imgui/imgui.h"
#include <SDL3/SDL.h>
using namespace std;

namespace Sokoban {
	struct GameData;
}

enum class HackAction {
	REMOVE = 0,
	ADD = 1,
};

struct UIPanel {
	float x;
	float y;
	float w;
	float h;
	bool dragging = false;
};

namespace HACKUI {
	extern Vector mouseStartPos;
	extern Vector mouseCurrentPos;
	void DrawUIPanel(UIPanel& panel, SDL_Renderer* renderer);
	void UpdateUIPanel(Sokoban::GameData* data, UIPanel& panel);
	void MovePanel(Sokoban::GameData* data, UIPanel* panel, float dx, float dy);
}
