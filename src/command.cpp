#include "command.h"

void Execute(AnyCommand cmd, bool from_redo = false) {
    switch (cmd.command.type) {
    case CMD_TYPE::NONE:
        break;
    case CMD_TYPE::MOVE: {
        MoveCommand mv = cmd.move;
        mv.entity->x_prev = mv.entity->x;
        mv.entity->y_prev = mv.entity->y;
        mv.entity->x += mv.xDir;
        mv.entity->y += mv.yDir;
        if (from_redo) {
            mv.entity->progress_01 = 1;
        }
        break;
    }
    case CMD_TYPE::ROTATE: {
        RotateCommand rotate = cmd.rotate;
        if (!HasBehaviour(rotate.entity, CAN_ROTATE)) {
            break;
        }
        rotate.entity->facing = rotate.to;
        break;
    }
    case CMD_TYPE::MODIFY_BEHAVIOUR: { // new
        ModifyBehaviourCommand modify = cmd.modify;
        if (modify.mode == ModifyBehaviourCommand::ADD) {
            AddBehaviour(modify.entity, modify.flag);
        }
        else {
            RemoveBehaviour(modify.entity, modify.flag);
        }
        break;
    }
    }
}

void Push(CommandBuffer* buffer, AnyCommand cmd) {
    buffer->allCommands[buffer->index] = cmd;
    buffer->allCommands[buffer->index].command.timestamp = buffer->command_timestamp;
    buffer->index++;
    buffer->head = buffer->index;
    Execute(cmd);
}

void Undo(CommandBuffer* buffer) {
    if (buffer->index == 0) {
        return;
    }
    buffer->index--;
    AnyCommand cmd = buffer->allCommands[buffer->index];
    uint32_t timestamp = cmd.command.timestamp;
    switch (cmd.command.type) {
    case CMD_TYPE::NONE:
        break;
    case CMD_TYPE::MOVE: {
        MoveCommand mv = cmd.move;
        mv.entity->x -= mv.xDir;
        mv.entity->y -= mv.yDir;
        mv.entity->progress_01 = 1;
        break;
    }
    case CMD_TYPE::ROTATE: {
        RotateCommand rotate = cmd.rotate;
        if (!HasBehaviour(rotate.entity, CAN_ROTATE)) {
            break;
        }
        rotate.entity->facing = rotate.from;
        break;
    }
    case CMD_TYPE::MODIFY_BEHAVIOUR: { // new
        ModifyBehaviourCommand modify = cmd.modify;
        if (modify.mode == ModifyBehaviourCommand::ADD) {
            RemoveBehaviour(modify.entity, modify.flag); // inverted
        }
        else {
            AddBehaviour(modify.entity, modify.flag);
        }
        break;
    }
    }
    if (buffer->index > 0) {
        if (buffer->allCommands[buffer->index - 1].command.timestamp == timestamp) {
            Undo(buffer);
        }
    }
}

void Redo(CommandBuffer* buffer) {
    AnyCommand cmd = buffer->allCommands[buffer->index];
    if (cmd.command.type == CMD_TYPE::NONE) {
        return;
    }
    if (buffer->index == buffer->head) {
        return;
    }
    buffer->index++;
    Execute(cmd,true);

    int timestamp = cmd.command.timestamp; 
    if (buffer->index != buffer->head) { 
        AnyCommand nextCommand = buffer->allCommands[buffer->index];
        if (nextCommand.command.timestamp == timestamp) {
            Redo(buffer);
        }
    }

}