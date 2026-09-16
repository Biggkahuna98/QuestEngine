#include "Core/Memory/Memory.h"

#include <cstdlib>
#include <iostream>

#if defined(_WIN32)
    #include <malloc.h>
#endif

namespace Quest
{
    static BasicMemoryStats g_Stats;

    // What plain malloc/operator new already guarantee (16 bytes on x64). Every allocation
    // goes through the aligned path, even at this default, so that a pointer handed out by
    // Malloc is always safe to hand back to Free - the two must agree on the allocator.
    static constexpr std::size_t kDefaultAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;

    void* Memory::Malloc(std::size_t size, std::size_t alignment)
    {
        if (alignment < kDefaultAlignment)
            alignment = kDefaultAlignment;

        // operator new(0) still has to return a unique pointer that Free can release.
        if (size == 0)
            size = 1;

        //std::cout << "Allocating " << size << " bytes with " << alignment << " byte alignment" << std::endl;
#if defined(_WIN32)
        void* ptr = _aligned_malloc(size, alignment);
#else
        // std::aligned_alloc wants a size that is a multiple of the alignment.
        size = (size + alignment - 1) & ~(alignment - 1);
        void* ptr = std::aligned_alloc(alignment, size);
#endif
        //std::cout << "Allocated memory at: " << ptr << std::endl;
        if (!ptr)
            return nullptr;

        g_Stats.Allocs++;
        return ptr;
    }

    void Memory::Free(void* ptr)
    {
        //std::cout << "Freeing memory at: " << ptr << std::endl;
        if (!ptr)
            return;

        g_Stats.Frees++;

#if defined(_WIN32)
        _aligned_free(ptr);
#else
        std::free(ptr);
#endif
    }

    BasicMemoryStats Memory::GetStats()
    {
        return g_Stats;
    }
}
