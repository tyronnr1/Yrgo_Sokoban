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

    entity.InitializeBaseBehaviour();

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