#include "Core/Memory/Memory.h"

#include <cstdlib>
#include <iostream>

namespace Quest
{
    static BasicMemoryStats g_Stats;

    void* Memory::Malloc(std::size_t size, std::uint32_t alignment)
    {
        g_Stats.Allocs++;

        uint32_t padding = 0;
        if (alignment == 0)
        {
            if (size >= 16)
                padding = 16 - (size % 16);
            else
                padding = 8 - (size % 8);
        }

        //std::cout << "Allocating " << size << " bytes" << "with " << padding << " padding bytes" << std::endl;
        void *ptr = std::malloc(size + padding);
        //std::cout << "Allocated memory at: " << ptr << std::endl;
        return ptr;
    }

    void Memory::Free(void* ptr)
    {
        //std::cout << "Freeing memory at: " << ptr << std::endl;
        if (!ptr)
            return;

        g_Stats.Frees++;

        std::free(ptr);
    }

    BasicMemoryStats Memory::GetStats()
    {
        return g_Stats;
    }
}
