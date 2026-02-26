#include "VulkanContext.h"

namespace qrhi::vulkan
{
    VulkanContext::VulkanContext(ContextDesc desc)
        : m_Desc(desc)
    {
    }

    VulkanContext::~VulkanContext()
    {
    }

    DeviceHandle VulkanContext::CreateDevice(DeviceDesc)
    {
        return DeviceHandle();
    }
}
