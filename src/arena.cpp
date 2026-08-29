#include "arena.h"
#include <cstring>

void Memory::Initialize(Arena* arena, void* memory, size_t size)
{
    arena->memory = memory;
    arena->size = size;
    arena->used = 0;
}

void* Memory::Allocate(Arena* arena, size_t size)
{
    if (arena->used + size > arena->size)
    {
        return nullptr; // Safety so we can't go beyond our arena size
    }

    void* front = static_cast<char*>(arena->memory) + arena->used;
    arena->used += size;

    memset(front, 0, size); // zero-allocation: no garbage data left behind
    return front;
}

void Memory::Reset(Arena* arena)
{
    arena->used = 0;
}

Memory::Arena* Memory::CreateSubArena(Arena* parent_arena, size_t size){
    Memory::Arena* sub_arena = (Memory::Arena*)Allocate(parent_arena,
    sizeof(Memory::Arena));
    void* memory_start = Allocate(parent_arena, size);
    Memory::Initialize(sub_arena, memory_start, size);
    return sub_arena;
}