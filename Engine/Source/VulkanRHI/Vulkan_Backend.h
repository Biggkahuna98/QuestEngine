#pragma once
#include <vulkan/vulkan.hpp>

#include <string_view>
#include <memory>

#include "RHI/CommandList.h"
#include "Utility/RefCounting.h"

#define VK_CHECK(res) assert((res) == vk::Result::eSuccess);

namespace qrhi::vulkan
{
    struct VulkanContext
    {
        VulkanContext(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device device)
            : instance(instance), physicalDevice(physicalDevice), device(device) {}

        vk::Instance instance;
        vk::PhysicalDevice physicalDevice;
        vk::Device device;

        vk::PhysicalDeviceProperties physicalDeviceProperties;

        void error(const std::string_view& message);
        void warning(const std::string_view& message);
        void info(const std::string_view& message);
    };

    class TrackedCommandBuffer
    {
    public:
        TrackedCommandBuffer(const VulkanContext& context)
            : context(context) {}
        ~TrackedCommandBuffer();

        // The actual vulkan command buffer/pool
        vk::CommandBuffer commandBuffer = vk::CommandBuffer();
        vk::CommandPool commandPool = vk::CommandPool();

    private:
        const VulkanContext& context;
    };

    using TrackedCommandBufferPtr = std::shared_ptr<TrackedCommandBuffer>;

    // Queue that represents a hardware queue
    class Queue
    {
    public:
        Queue(const VulkanContext& context, QueueType queueID, vk::Queue queue, uint32_t queueFamilyIndex);
        ~Queue();

        // Create a command buffer
        TrackedCommandBufferPtr CreateCommandBuffer();
        TrackedCommandBufferPtr GetOrCreateCommandBuffer();

        void AddWaitSemaphore(vk::Semaphore semaphore, uint64_t value);
        void AddSignalSemaphore(vk::Semaphore semaphore, uint64_t value);

        // Submit a command buffer to this queue, return the submission id
        uint64_t Submit(CommandList* cmd, size_t numCmd);
    private:
        const VulkanContext& context;

        vk::Queue queue;

    };
}