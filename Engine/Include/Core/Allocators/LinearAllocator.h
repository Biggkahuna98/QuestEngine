#pragma once
#include "AllocatorBase.h"

namespace QE
{
    class QUEST_API LinearAllocator : public AllocatorBase
    {
    public:
        LinearAllocator(const size_t sizeInBytes, void* const addressOfAllocatedMemory) noexcept;
        LinearAllocator(const LinearAllocator&) = delete;
        LinearAllocator& operator=(const LinearAllocator&) = delete;
        LinearAllocator(LinearAllocator&&) noexcept;
        LinearAllocator& operator=(LinearAllocator&&) noexcept;
        ~LinearAllocator() noexcept override;

        void* Allocate(const size_t sizeInBytes, const std::uintptr_t alignment) override;
        void Free(void* const ptr) noexcept override;

        [[nodiscard]] void* GetCurrentPointer() const noexcept;

        virtual void Rewind(void* const mark) noexcept;
        virtual void Clear() noexcept;
    protected:
        void* m_CurrentPtr;
    };

}

