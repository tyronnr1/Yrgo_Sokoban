#pragma once

#include <string>
#include <vector>
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
};

namespace HACKUI {

	void DrawUIPanel(UIPanel& panel, SDL_Renderer* renderer);
	void UpdateUIPanel(Sokoban::GameData* data, UIPanel& panel);
}
