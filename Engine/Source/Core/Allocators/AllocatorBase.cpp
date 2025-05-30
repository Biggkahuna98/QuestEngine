#include "Core/Allocators/AllocatorBase.h"
#include "Core/Log.h"

namespace QE
{
    AllocatorBase::AllocatorBase(const std::size_t sizeInBytes, void *const addressOfAllocatedMemory) noexcept
        : m_TotalBytes(sizeInBytes), m_UsedBytes(0), m_NumAllocations(0), m_StartPtr(addressOfAllocatedMemory)
    {
        // TODO: make an assert macro later and test it that sizeInBytes > 0
        if (!sizeInBytes > 0)
        {
            LOG_ERROR("Size of memory to allocate is 0");
            abort();
        }
    }

    AllocatorBase::AllocatorBase(AllocatorBase&& other) noexcept
        : m_TotalBytes(other.m_TotalBytes), m_UsedBytes(other.m_UsedBytes), m_NumAllocations(other.m_NumAllocations), m_StartPtr(other.m_StartPtr)
    {
        other.m_TotalBytes = 0;
        other.m_UsedBytes = 0;
        other.m_NumAllocations = 0;
        other.m_StartPtr = nullptr;
    }

    AllocatorBase& AllocatorBase::operator=(AllocatorBase&& rhs) noexcept
    {
        m_TotalBytes = rhs.m_TotalBytes;
        m_UsedBytes = rhs.m_UsedBytes;
        m_NumAllocations = rhs.m_NumAllocations;
        m_StartPtr = rhs.m_StartPtr;

        rhs.m_TotalBytes = 0;
        rhs.m_UsedBytes = 0;
        rhs.m_NumAllocations = 0;
        rhs.m_StartPtr = nullptr;

        return *this;
    }

    AllocatorBase::~AllocatorBase() noexcept
    {
        // TODO: maybe deallocate the startptr here, look into it later
    }

    std::size_t AllocatorBase::GetTotalSize() const noexcept
    {
        return m_TotalBytes;
    }

    std::size_t AllocatorBase::GetUsedSize() const noexcept
    {
        return m_UsedBytes;
    }

    std::size_t AllocatorBase::GetFreeSize() const noexcept
    {
        return m_TotalBytes - m_UsedBytes;
    }

    std::size_t AllocatorBase::GetNumberOfAllocations() const noexcept
    {
        return m_NumAllocations;
    }

    const void* AllocatorBase::GetStartPtr() const noexcept
    {
        return m_StartPtr;
    }
}