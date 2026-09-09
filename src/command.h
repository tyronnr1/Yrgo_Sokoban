#pragma once
#include <cstdint>
#include "entity.h"
#include "hackUi.h"

enum class CMD_TYPE : uint8_t {
    NONE = 0,
    MOVE = 1,
    //HACK = 2
};

struct Command {
    CMD_TYPE type;
    uint32_t timestamp;
};

struct MoveCommand : Command {
    Entity* entity;
    int xDir;
    int yDir;
}; 

struct HackLine {
    Entity* e;
    Behaviour bh;
    HackAction action; // ADD or REMOVE
};

struct CompileCommand : Command {
    HackLine lines[10];
    int lineCount;
};

union AnyCommand {
    Command command;
    MoveCommand move;
    //CompileCommand compile;

    AnyCommand(MoveCommand mv) {
        move = mv;
    }

 //   AnyCommand(CompileCommand cmd) {
 //       compile = cmd;
	//}

};

struct CommandBuffer {
    AnyCommand* allCommands;
    int capacity;
    int index; // where we currently are in the chain
    int head;  // the furthest point we've reached (limit for redo)
};



void Push(CommandBuffer* buffer, AnyCommand cmd, uint32_t timestamp);
void Undo(CommandBuffer* buffer);
void Redo(CommandBuffer* buffer);