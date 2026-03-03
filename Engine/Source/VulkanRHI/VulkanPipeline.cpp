#include "VulkanPipeline.h"

#include "VulkanContext.h"

namespace qrhi::vulkan
{
    // Graphics pipeline
    VulkanGraphicsPipeline::VulkanGraphicsPipeline(GraphicsPipelineDesc desc, const VulkanContext* context)
        : m_Context(context), m_Desc(desc)
    {
    }

    VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
    {
    }

    Quest::OpaqueObject VulkanGraphicsPipeline::GetNativeType()
    {
        Quest::OpaqueObject obj;
        obj.pointer = static_cast<void*>(this);
        return obj;
    }

    void VulkanGraphicsPipeline::SetDebugName(const std::string& name)
    {
        m_Context->SetDebugName(m_Pipeline, vk::ObjectType::ePipeline, name);
    }
    
    // Compute pipeline
    VulkanComputePipeline::VulkanComputePipeline(ComputePipelineDesc desc, const VulkanContext* context)
        : m_Context(context), m_Desc(desc)
    {
    }

    VulkanComputePipeline::~VulkanComputePipeline()
    {
    }

    Quest::OpaqueObject VulkanComputePipeline::GetNativeType()
    {
        Quest::OpaqueObject obj;
        obj.pointer = static_cast<void*>(this);
        return obj;
    }

    void VulkanComputePipeline::SetDebugName(const std::string& name)
    {
        m_Context->SetDebugName(m_Pipeline, vk::ObjectType::ePipeline, name);
    }
}
