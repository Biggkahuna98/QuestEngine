#include "Core/Allocators/LinearAllocator.h"

#include "Core/Core.h"
#include "Core/Log.h"
#include <utility>

namespace QE
{
    inline std::size_t align_forward_adjustment(const void* const ptr, const std::size_t & alignment) noexcept
    {
        const auto iptr = reinterpret_cast<std::uintptr_t>(ptr);
        const auto aligned = (iptr - 1u + alignment) & -alignment;
        return aligned - iptr;
    }

    inline void* ptr_add(const void* const p, const std::uintptr_t& amount) noexcept
    {
        return reinterpret_cast<void*>
            (reinterpret_cast<std::uintptr_t>(p) + amount);
    }

    LinearAllocator::LinearAllocator(const size_t sizeInBytes, void * const addressOfAllocatedMemory) noexcept
        : AllocatorBase(sizeInBytes, addressOfAllocatedMemory), m_CurrentPtr(addressOfAllocatedMemory)
    {
    }

    LinearAllocator::LinearAllocator(LinearAllocator&& other) noexcept
        : AllocatorBase(std::move(other)), m_CurrentPtr(other.m_CurrentPtr)
    {
        other.m_CurrentPtr = nullptr;
    }

    LinearAllocator & LinearAllocator::operator=(LinearAllocator&& rhs) noexcept
    {
        AllocatorBase::operator=(std::move(rhs));
        m_CurrentPtr = rhs.m_CurrentPtr;
        rhs.m_CurrentPtr = nullptr;
        return *this;
    }

    LinearAllocator::~LinearAllocator() noexcept
    {
        LinearAllocator::Clear();
    }

    void* LinearAllocator::Allocate(const size_t sizeInBytes, const std::uintptr_t alignment)
    {
        QE_ASSERT(sizeInBytes > 0 && alignment > 0);

        size_t adjustment = align_forward_adjustment(m_CurrentPtr, alignment);
    }

    void LinearAllocator::Free(void * const ptr) noexcept
    {
    }

    void* LinearAllocator::GetCurrentPointer() const noexcept
    {
        return nullptr;
    }

    void LinearAllocator::Rewind(void * const mark) noexcept
    {
    }

    void LinearAllocator::Clear() noexcept
    {
    }
}
