#pragma once
#include "Core/Core.h"

// https://indiegamedev.net/2022/03/27/custom-c20-memory-allocators-for-stl-containers/
// https://johnfarrier.com/custom-allocators-in-c-high-performance-memory-management/
// https://github.com/mtrebi/memory-allocators/tree/master

namespace QE
{
    class QUEST_API AllocatorBase
    {
    public:
        AllocatorBase(const std::size_t sizeInBytes, void* const addressOfAllocatedMemory) noexcept;
        virtual ~AllocatorBase() noexcept;

        AllocatorBase(const AllocatorBase&) = delete;
        AllocatorBase& operator=(const AllocatorBase&) = delete;
        AllocatorBase(AllocatorBase&& other) noexcept;
        AllocatorBase& operator=(AllocatorBase&& rhs) noexcept;

        virtual void* Allocate(const std::size_t sizeInBytes, const std::uintptr_t alignment = sizeof(std::intptr_t)) = 0;
        virtual void Free(void* const ptr) = 0;

        [[nodiscard]] std::size_t GetTotalSize() const noexcept;
        [[nodiscard]] std::size_t GetUsedSize() const noexcept;
        [[nodiscard]] std::size_t GetFreeSize() const noexcept;
        [[nodiscard]] std::size_t GetNumberOfAllocations() const noexcept;

        [[nodiscard]] const void* GetStartPtr() const noexcept;
    protected:
        std::size_t m_TotalBytes;
        std::size_t m_UsedBytes;
        std::size_t m_NumAllocations;
        void* m_StartPtr;
    };
}