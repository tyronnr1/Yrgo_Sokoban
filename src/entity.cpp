#include "entity.h"
#include "levels.h"

bool IsMoving(Entity* e) {
    return e->x != e->x_prev || e->y != e->y_prev;
}

void AddEntity(ID id, int x, int y, LevelData* level)
{
    if (level->entityCount >= level->entityCapacity) {
        return;
    }

    Entity& entity = level->entityBuffer[level->entityCount];

    entity.id = id;
    entity.x = x;
    entity.y = y;
    entity.x_prev = x;
    entity.y_prev = y;
    entity.progress_01 = 0.0f;

    InitializeBaseBehaviour(&entity);

    level->entityCount++;
}

void RemoveEntity(int x, int y, LevelData* level)
{
    for (int i = 0; i < level->entityCount; i++) {
        if (level->entityBuffer[i].x == x &&
            level->entityBuffer[i].y == y) {

            for (int j = i; j < level->entityCount - 1; j++) {
                level->entityBuffer[j] = level->entityBuffer[j + 1];
            }

            level->entityCount--;
            return;
        }
    }
}

bool HasBehaviour(Entity* entity, Behaviour flags) {
	return (entity->behaviour & flags) == flags;
}
void SetBehaviour(Entity* entity, Behaviour flags) {
	entity->behaviour = flags;
}
void AddBehaviour(Entity* entity, Behaviour flags) {
	entity->behaviour = (Behaviour)(entity->behaviour | flags);
}
void RemoveBehaviour(Entity* entity, Behaviour flags) {
	entity->behaviour = (Behaviour)(entity->behaviour & ~flags);
}

void InitializeBaseBehaviour(Entity* entity) {
	assert(entity->id != ID::NONE);
	entity->strength = 999;
	switch (entity->id) {
	default:
		SetBehaviour(entity, NONE);
		break;
	case ID::PLAYER:
		SetBehaviour(entity, (Behaviour)(CAN_MOVE | IS_PLAYER | RESPOND_TO_INPUT | CAN_ROTATE));
		break;
	case ID::BOX_1:
		SetBehaviour(entity, (Behaviour)(CAN_MOVE));
		break;
	case ID::BOX_METAL:
		SetBehaviour(entity, (Behaviour)(CAN_MOVE | IS_HEAVY));
		break;
	case ID::SPIKE_DOWN:
		SetBehaviour(entity, (Behaviour)(CAN_WALK_THROUGH));
		break;
	case ID::SPIKE:
		SetBehaviour(entity, (Behaviour)(CAN_WALK_THROUGH | KILLS_PLAYER));
		break;
	}
}