#pragma once

#include "VulkanCommon.h"
#include "Core/RHI/SyncObject.h"

namespace Quest::Vulkan
{
    // Timeline semaphore backing for the RHI SyncObject.
    class VulkanSyncObject : public SyncObject
    {
    public:
        VulkanSyncObject(const VulkanContext* context, uint64_t initialValue, const std::string& debugName = "Timeline Semaphore");
        ~VulkanSyncObject() override;

        void Wait(uint64_t value) override;
        uint64_t GetCompletedValue() override;

        vk::Semaphore GetSemaphore() const { return m_Semaphore; }

    private:
        const VulkanContext* m_Context;

        vk::Semaphore m_Semaphore;
    };
}
