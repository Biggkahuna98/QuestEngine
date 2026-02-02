#include "RHI/Device.h"

// For vulkan
#include "VulkanRHI/Vulkan_Device.h"

namespace qrhi
{
    /*DeviceHandle CreateDevice(const DeviceDesc& desc)
    {
        if (desc.api == GraphicsAPI::Vulkan)
        {
            LOG_INFO("Creating Vulkan Device");
            return std::make_shared<qrhi::vulkan::VulkanDevice>(desc);
        }
        return nullptr;
        // Only one option for GraphicsAPI to be set to... will add a void RHI implementation at some point as a default
    }*/
}