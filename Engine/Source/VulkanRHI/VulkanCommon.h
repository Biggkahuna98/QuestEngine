#pragma once

#include "Core/Log.h"
#include "RHI/QRHI.h"

#include "VMA_Usage.h"

#include <vulkan/vulkan.hpp>
#include <vulkan/vk_enum_string_helper.h>

#define VK_CHECK_RES(res) assert((res) == vk::Result::eSuccess);

#define VK_CHECK(x)                                                                      \
do {                                                                                     \
    vk::Result err = x;                                                                  \
    if (static_cast<uint32_t>(err)) {                                                    \
        LOG_DEBUG_TAG("Vulkan", "Detected Vulkan error: {}", vk::to_string(err));        \
        abort();                                                                         \
    }                                                                                    \
} while (0)

// Old C one for any needs, like VMA
#define VK_CHECK_OLD(x)                                                                  \
do {                                                                                     \
    VkResult err = x;                                                                    \
    if (err) {                                                                           \
        LOG_DEBUG_TAG("Vulkan", "Detected Vulkan error: {}", string_VkResult(err));      \
        abort();                                                                         \
    }                                                                                    \
} while (0)

namespace qrhi::vulkan
{
    class VulkanContext;

    class TrackedCommandBuffer final
    {
    public:
        vk::CommandPool commandPool = vk::CommandPool();
        vk::CommandBuffer commandBuffer = vk::CommandBuffer();

        std::vector<Quest::RefCountPtr<qrhi::Resource>> trackedResources;

        uint64_t recordingID = 0;
        uint64_t submissionID = 0;

        TrackedCommandBuffer(const VulkanContext* context)
            : m_Context(context) {}
        ~TrackedCommandBuffer();
    private:
        const VulkanContext* m_Context;
    };

    using TrackedCommandBufferPtr = std::shared_ptr<TrackedCommandBuffer>;

    // Hardware queue wrapper
    class Queue final
    {
    public:
        Queue(const VulkanContext* context, QueueType id, vk::Queue queue, uint32_t queueFamilyIndex);
        ~Queue();

        TrackedCommandBufferPtr CreateTrackedCommandBuffer();
        TrackedCommandBufferPtr GetOrCreateTrackedCommandBuffer();

        void AddWaitSemaphore(vk::Semaphore semaphore, uint64_t value);
        void AddSignalSemaphore(vk::Semaphore semaphore, uint64_t value);

        uint64_t Submit(CommandList* commandList, vk::Semaphore waitSemaphore, vk::Semaphore signalSemaphore,
            vk::Fence fence);

        QueueType GetID() const { return m_ID; }
        vk::Queue GetQueue() const { return m_Queue; }
        vk::Semaphore GetTrackingSemaphore() const { return m_TrackingSemaphore; }
        uint64_t GetLastSubmittedID() const { return m_LastSubmittedID; }
    private:
        const VulkanContext* m_Context;
        vk::Queue m_Queue;
        QueueType m_ID;
        uint32_t m_QueueFamilyIndex = -1;

        vk::Semaphore m_TrackingSemaphore;
        uint64_t m_LastSubmittedID = 0;

        std::mutex m_Mutex;

        std::vector<vk::Semaphore> m_WaitSemaphores;
        std::vector<vk::Semaphore> m_SignalSemaphores;
        std::vector<uint64_t> m_WaitSemaphoreValues;
        std::vector<uint64_t> m_SignalSemaphoreValues;

        std::vector<TrackedCommandBufferPtr> m_TrackedCommandBuffersInFlight;
    };

    // If you need device backed memory, inherit from this
    class MemoryResource
    {
    public:
        VmaAllocation allocation = nullptr;
        VmaAllocationInfo allocationInfo;
    };
}