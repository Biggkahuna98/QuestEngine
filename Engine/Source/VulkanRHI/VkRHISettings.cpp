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

    VkShaderStageFlags ShaderStageFromRHI(ShaderStage stage)
    {
        switch (stage)
        {
            case ShaderStage::Vertex:
                return VK_SHADER_STAGE_VERTEX_BIT;
            case ShaderStage::Fragment:
                return VK_SHADER_STAGE_FRAGMENT_BIT;
            case ShaderStage::Compute:
                return VK_SHADER_STAGE_COMPUTE_BIT;
            default:
                return FAILURE_CODE_FROM_VK_ENUM;
        }
    }

    VkShaderStageFlagBits ShaderStageFlagBitsFromRHI(ShaderStage stage)
    {
        switch (stage)
        {
            case ShaderStage::Vertex:
                return VK_SHADER_STAGE_VERTEX_BIT;
            case ShaderStage::Fragment:
                return VK_SHADER_STAGE_FRAGMENT_BIT;
            case ShaderStage::Compute:
                return VK_SHADER_STAGE_COMPUTE_BIT;
            default:
                return VK_SHADER_STAGE_ALL; // idk
        }
    }

    VkPrimitiveTopology PrimitiveTopologyFromRHI(PipelineTopology topology)
    {
        switch (topology)
        {
            case PipelineTopology::PointList:
                return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            case PipelineTopology::LineList:
                return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case PipelineTopology::LineStrip:
                return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            case PipelineTopology::LineListAdjacency:
                return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
            case PipelineTopology::LineStripAdjacency:
                return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
            case PipelineTopology::TriangleList:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            case PipelineTopology::TriangleStrip:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            case PipelineTopology::TriangleFan:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
            case PipelineTopology::TriangleListAdjacency:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
            case PipelineTopology::TriangleStripAdjacency:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
            case PipelineTopology::PatchList:
                return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
            default:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        }
    }

    VkPolygonMode PolygonModeFromRHI(PolygonMode mode)
    {
       switch (mode)
       {
           case PolygonMode::Point:
               return VK_POLYGON_MODE_POINT;
           case PolygonMode::Line:
               return VK_POLYGON_MODE_LINE;
           case PolygonMode::Fill:
               return VK_POLYGON_MODE_FILL;
           default:
               return VK_POLYGON_MODE_FILL;
       }
    }

    VkCullModeFlags CullModeFromRHI(CullMode mode)
    {
        switch (mode)
        {
            case CullMode::None:
                return VK_CULL_MODE_NONE;
            case CullMode::Front:
                return VK_CULL_MODE_FRONT_BIT;
            case CullMode::Back:
                return VK_CULL_MODE_BACK_BIT;
            case CullMode::FrontBack:
                return VK_CULL_MODE_FRONT_AND_BACK;
            default:
                return VK_CULL_MODE_NONE;
        }
    }

    VkFrontFace FaceWindingOrderFromRHI(WindingOrder order)
    {
        switch (order)
        {
            case WindingOrder::Clockwise:
                return VK_FRONT_FACE_CLOCKWISE;
            case WindingOrder::CounterClockwise:
                return VK_FRONT_FACE_COUNTER_CLOCKWISE;
            default:
                return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        }
    }

    VkCompareOp DepthCompareOpFromRHI(DepthCompareOp op)
    {
        switch (op)
        {
            case DepthCompareOp::Never:
                return VK_COMPARE_OP_NEVER;
            case DepthCompareOp::Less:
                return VK_COMPARE_OP_LESS;
            case DepthCompareOp::Equal:
                return VK_COMPARE_OP_EQUAL;
            case DepthCompareOp::LessEqual:
                return VK_COMPARE_OP_LESS_OR_EQUAL;
            case DepthCompareOp::Greater:
                return VK_COMPARE_OP_GREATER;
            case DepthCompareOp::NotEqual:
                return VK_COMPARE_OP_NOT_EQUAL;
            case DepthCompareOp::GreaterEqual:
                return VK_COMPARE_OP_GREATER_OR_EQUAL;
            case DepthCompareOp::Always:
                return VK_COMPARE_OP_ALWAYS;
            default:
                return VK_COMPARE_OP_NEVER;
        }
    }
}
