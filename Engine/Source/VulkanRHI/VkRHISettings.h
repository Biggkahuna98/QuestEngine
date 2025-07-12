#pragma once

#include "RHI/RHISettings.h"
#include <vulkan/vulkan.h>
#include <limits>

namespace QE
{
    // This should be entirely unreachable from the converter functions
    // Default cases from these functions should never be reached
    inline unsigned int FAILURE_CODE_FROM_VK_ENUM = std::numeric_limits<unsigned int>::max();

    // Buffer
    VkBufferUsageFlags BufferUsageFlagsFromRHI(BufferUsage usage);
    VkBufferUsageFlags BufferTypeFlagsFromRHI(BufferType type);

    // Shader
    VkShaderStageFlags ShaderStageFromRHI(ShaderStage stage);
    VkShaderStageFlagBits ShaderStageFlagBitsFromRHI(ShaderStage stage);

    // Pipeline
    VkPrimitiveTopology PrimitiveTopologyFromRHI(PipelineTopology topology);
    VkPolygonMode PolygonModeFromRHI(PolygonMode mode);
    VkCullModeFlags CullModeFromRHI(CullMode mode);
    VkFrontFace FaceWindingOrderFromRHI(WindingOrder order);
    VkCompareOp DepthCompareOpFromRHI(DepthCompareOp op);
}