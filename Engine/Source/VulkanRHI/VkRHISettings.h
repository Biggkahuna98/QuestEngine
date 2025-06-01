#pragma once

#include "RHI/RHISettings.h"
#include <vulkan/vulkan.h>
#include <limits>

namespace QE
{
    // This should be entirely unreachable from the converter functions
    inline unsigned int FAILURE_CODE_FROM_VK_ENUM = std::numeric_limits<unsigned int>::max();

    // Buffer
    VkBufferUsageFlags BufferUsageFlagsFromRHI(BufferUsage usage);
    VkBufferUsageFlags BufferTypeFlagsFromRHI(BufferType type);
}