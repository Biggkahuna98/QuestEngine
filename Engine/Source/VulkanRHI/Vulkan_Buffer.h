#pragma once
#include "Vulkan_Backend.h"

namespace qrhi::vulkan
{
    class VulkanBuffer : public MemoryResource, public qrhi::Buffer
    {
    public:
        BufferDesc desc;

        vk::Buffer buffer;
        vk::DeviceAddress deviceAddress = 0;

        // For staging buffers
        QueueType lastUsedQueue = QueueType::Graphics;
        uint64_t lastUsedCommandListID;

        VulkanBuffer(const VulkanContext& context)
            : m_Context(context) {}
        ~VulkanBuffer() override;

        const BufferDesc& GetDesc() const override { return desc; }
        GpuVirtualAddress GetGpuVirtualAddress() const override { return deviceAddress; }
    private:
        const VulkanContext& m_Context;
    };
}