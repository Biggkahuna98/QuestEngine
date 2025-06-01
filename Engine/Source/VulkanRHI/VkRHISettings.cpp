#include "VkRHISettings.h"

namespace QE
{
    VkBufferUsageFlags BufferUsageFlagsFromRHI(BufferUsage usage)
    {
        switch (usage)
        {
            case BufferUsage::Default:
                return VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            case BufferUsage::Upload:
                return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            case BufferUsage::Readback:
                return VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            default:
                return FAILURE_CODE_FROM_VK_ENUM;
        }
    }

    VkBufferUsageFlags BufferTypeFlagsFromRHI(BufferType type)
    {
        switch (type)
        {
            case BufferType::Vertex:
                return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            case BufferType::Index:
                return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            case BufferType::Uniform:
                return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            case BufferType::Transfer:
                return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            default:
                return FAILURE_CODE_FROM_VK_ENUM;
        }
    }
}