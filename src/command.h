#pragma once
#include <cstdint>
#include "entity.h"
#include "hackUi.h"

enum class CMD_TYPE : uint8_t {
    NONE = 0,
    MOVE = 1,
    ROTATE = 2,
    MODIFY_BEHAVIOUR = 3,
    //HACK = 2
};

struct Command {
    CMD_TYPE type = CMD_TYPE::NONE;
    uint32_t timestamp;
};

struct MoveCommand : Command {
    Entity* entity;
    int xDir;
    int yDir;

    MoveCommand() {}
    MoveCommand(Entity* entity, int xDir, int yDir) {
        this->entity = entity;
        this->xDir = xDir;
        this->yDir = yDir;
        type = CMD_TYPE::MOVE;
    }
};

struct RotateCommand : Command {
    Entity* entity;
    Direction from;
    Direction to;

    RotateCommand(Entity* entity, Direction from, Direction to) {
        this->entity = entity;
        this->from = from;
        this->to = to;
        type = CMD_TYPE::ROTATE;
    }
};

struct ModifyBehaviourCommand : Command {
    enum Mode { ADD, REMOVE };

    Entity* entity;
    Behaviour flag;
    Mode mode;

    ModifyBehaviourCommand(Entity* entity, Behaviour flag, Mode mode) {
        this->entity = entity;
        this->flag = flag;
        this->mode = mode;
        type = CMD_TYPE::MODIFY_BEHAVIOUR;
    }
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
    RotateCommand rotate;
    ModifyBehaviourCommand modify;
    //CompileCommand compile;

    AnyCommand(MoveCommand mv) { move = mv; }
    AnyCommand(RotateCommand rc) { rotate = rc; }
    AnyCommand(ModifyBehaviourCommand mc) { modify = mc; }
    //   AnyCommand(CompileCommand cmd) { compile = cmd; }
};

struct CommandBuffer {
    AnyCommand* allCommands;
    int capacity;
    int index;
    int head;
    uint32_t command_timestamp;
};

void Push(CommandBuffer* buffer, AnyCommand cmd);
void Undo(CommandBuffer* buffer);
void Redo(CommandBuffer* buffer);