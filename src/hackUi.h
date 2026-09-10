#pragma once

#include <string>
#include <vector>
#include "imgui/imgui.h"
#include <SDL3/SDL.h>
using namespace std;

enum class HackAction {
	REMOVE = 0,
	ADD = 1,
};

struct UIPanel {

};

namespace HACKUI {

	void DrawUIPanel(UIPanel& panel);

}
