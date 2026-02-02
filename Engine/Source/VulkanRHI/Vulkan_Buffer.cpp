#include "Vulkan_Buffer.h"

namespace qrhi::vulkan
{
    VulkanBuffer::~VulkanBuffer()
    {
        vmaDestroyBuffer(m_Context.allocator, buffer, allocation);
    }
}
