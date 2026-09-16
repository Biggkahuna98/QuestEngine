#pragma once

#include <cstddef>
#include <cstdint>

#include "Core/CoreExport.h"

namespace Quest
{
    struct BasicMemoryStats
    {
        uint64_t Allocs = 0;
        uint64_t Frees = 0;
    };

    struct Memory
    {
        CORE_API static void* Malloc(std::size_t size, std::uint32_t alignment = 0);
        CORE_API static void Free(void* ptr);
        CORE_API static BasicMemoryStats GetStats();
    };
}