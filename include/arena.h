#pragma once
#include <cstddef>

namespace Memory
{
    struct Arena
    {
        void* memory;
        size_t size;
        size_t used;
    };

    void Initialize(Arena* arena, void* memory, size_t size);
    void* Allocate(Arena* arena, size_t size);
    void Reset(Arena* arena);

    Arena* CreateSubArena(Arena* parent_arena, size_t size);

}

#define ALLOC(arena, type) (type*)Memory::Allocate((arena), sizeof(type))
#define ALLOC_ARRAY(arena, type, count) (type*)Memory::Allocate((arena), sizeof(type) * (count))