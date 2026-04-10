#include "VulkanBuffer.h"

#include "VulkanContext.h"

namespace qrhi::vulkan
{
    vk::BufferUsageFlagBits ConvertBufferType(BufferType type)
    {
        switch (type)
        {
        case BufferType::Vertex:
            return vk::BufferUsageFlagBits::eVertexBuffer;
        case BufferType::Index:
            return vk::BufferUsageFlagBits::eIndexBuffer;
        case BufferType::Uniform:
            return vk::BufferUsageFlagBits::eUniformBuffer;
        }
    }

    VulkanBuffer::VulkanBuffer(BufferDesc desc, VulkanContext* context)
        : m_Desc(desc), m_Context(context)
    {
        vk::BufferCreateInfo bufferInfo {
            .size = desc.sizeInBytes,
            .usage = ConvertBufferType(desc.type) | vk::BufferUsageFlagBits::eShaderDeviceAddress,
            .sharingMode = vk::SharingMode::eExclusive,
        };
        std::string size = "Sizeinbytes: " + std::to_string(desc.sizeInBytes);
        m_Context->GetLog()->Info(size);

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

        VkBuffer buff;
        auto info = static_cast<VkBufferCreateInfo>(bufferInfo);
        VK_CHECK_OLD(vmaCreateBuffer(m_Context->GetAllocator(), &info, &allocInfo, &buff, &allocation, &allocationInfo));
        m_Buffer = buff;

        vk::BufferDeviceAddressInfo addressInfo = {};
        addressInfo.buffer = m_Buffer;
        m_DeviceAddress = m_Context->GetDevice().getBufferAddress(addressInfo);

        if (m_DeviceAddress == 0)
        {
            m_Context->logError("Buffer Device Address is 0");
        }
    }

    VulkanBuffer::~VulkanBuffer()
    {
        m_Context->GetDevice().waitIdle();
        m_Context->GetDevice().destroyBuffer(m_Buffer);
        vmaFreeMemory(m_Context->GetAllocator(), allocation);
    }

    Quest::OpaqueObject VulkanBuffer::GetNativeType()
    {
        Quest::OpaqueObject obj;
        obj.pointer = static_cast<void*>(this);
        return obj;
    }

    void VulkanBuffer::SetDebugName(const std::string& name)
    {
        m_Context->SetDebugName(m_Buffer, vk::ObjectType::eBuffer, name);
    }
}
