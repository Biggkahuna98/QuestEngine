#pragma once

#include "VulkanCommon.h"

namespace qrhi::vulkan
{
    class VulkanBuffer : public Buffer, public MemoryResource
    {
    public:
        VulkanBuffer(BufferDesc desc, VulkanContext* context);
        ~VulkanBuffer() override;

        vk::Buffer GetBuffer() { return m_Buffer; }
        vk::DeviceAddress GetDeviceAddress() { return m_DeviceAddress; }

        Quest::OpaqueObject GetNativeType() override;
        void SetDebugName(const std::string& name) override;
        const BufferDesc& GetDesc() const override { return m_Desc; };

    private:
        BufferDesc m_Desc;
        VulkanContext* m_Context;

        vk::Buffer m_Buffer;
        vk::DeviceAddress m_DeviceAddress;
    };
}