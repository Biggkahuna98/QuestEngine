#include "RHI/Context.h"

// API contexts
#include "VulkanRHI/VulkanContext.h"

namespace qrhi
{
    ContextHandle CreateContext(ContextDesc desc)
    {
        switch (desc.graphicsAPI)
        {
        case API::Vulkan: return Quest::MakeRefCounted<qrhi::vulkan::VulkanContext>(desc);
        case API::None: return nullptr;
        }
    }
}