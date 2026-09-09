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

// -----------------------------------------------------------------------
// Block: one draggable item with its own texture. `placedInSlot == -1`
// means it's sitting in its home tray, not placed in any slot.
// -----------------------------------------------------------------------
struct Block {
    int id;
    std::string label;      // used for the drag payload / debug, not drawn
    SDL_Texture* texture;   // your own art, loaded via SDL_image as usual
    int placedInSlot = -1;  // -1 = not placed anywhere
};

// -----------------------------------------------------------------------
// Slot: a fixed drop target. `filledBlockId == -1` means empty.
// -----------------------------------------------------------------------
struct Slot {
    int id;
    ImVec2 size;
    SDL_Texture* emptyTexture;   // art shown when nothing is placed
    SDL_Texture* filledTexture;  // optional: art shown as a "socket" outline when filled
    int filledBlockId = -1;
};

// -----------------------------------------------------------------------
// UIPanel: the collapsible, draggable window.
// Collapse behavior: when collapsed, only a thin top strip (with the
// collapse button) is visible — the rest of the content is simply not
// drawn and the window is resized to ~10% of its full height.
// Drag behavior: dragging only works when the mouse is pressed down
// inside the top margin strip, not anywhere else in the panel.
// -----------------------------------------------------------------------
struct UIPanel {

};

namespace HACKUI {

	void DrawUIPanel(UIPanel& panel);

}
