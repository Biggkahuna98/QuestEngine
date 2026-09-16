#include "VulkanRHI/VulkanGraphicsDevice.h"

namespace Quest::Vulkan
{
    Device::Device(const DeviceDesc& desc)
        : m_Desc(desc)
    {
    }

    Device::~Device()
    {
    }

    void Device::WaitIdle()
    {
    }
}
