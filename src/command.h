#pragma once
#include <cstdint>
#include "entity.h"
#include "hackUi.h"

struct LevelData;

enum class CMD_TYPE : uint8_t {
    NONE = 0,
    MOVE = 1,
    ROTATE = 2,
    MODIFY_BEHAVIOUR = 3,
    ADD = 4,
    REMOVE = 5
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

struct AddCommand : Command {
    int x;
    int y;
    ID id;

    AddCommand(int x, int y, ID id) {
        this->x = x;
        this->y = y;
        this->id = id;
        type = CMD_TYPE::ADD;
    }
};

struct RemoveCommand : Command {
    int x;
    int y;
    Behaviour storedBehaviour;
    ID storedID;

    RemoveCommand(Entity* entity) {
        x = entity->x;
        y = entity->y;
        storedBehaviour = entity->behaviour;
        storedID = entity->id;
        type = CMD_TYPE::REMOVE;
    }
};

struct HackLine {
    Entity* e;
    Behaviour bh;
    HackAction action;
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
    AddCommand add;
    RemoveCommand remove; 
    //CompileCommand compile;

    AnyCommand(MoveCommand mv) { move = mv; }
    AnyCommand(RotateCommand rc) { rotate = rc; }
    AnyCommand(ModifyBehaviourCommand mc) { modify = mc; }
    AnyCommand(AddCommand ac) { add = ac; }
    AnyCommand(RemoveCommand rc) { remove = rc; }
    //   AnyCommand(CompileCommand cmd) { compile = cmd; }
};

struct CommandBuffer {
    AnyCommand* allCommands;
    int capacity;
    int index;
    int head;
    uint32_t command_timestamp;
};

void Push(CommandBuffer* buffer, AnyCommand cmd, LevelData* level);
void Undo(CommandBuffer* buffer, LevelData* level);
void Redo(CommandBuffer* buffer, LevelData* level);