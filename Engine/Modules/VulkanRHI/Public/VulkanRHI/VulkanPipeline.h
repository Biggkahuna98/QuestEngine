#pragma once

#include "VulkanCommon.h"
#include "Core/RHI/Pipeline.h"

namespace Quest::Vulkan
{
    class VulkanGraphicsPipeline : public GraphicsPipeline
    {
    public:
        VulkanGraphicsPipeline(GraphicsPipelineDesc desc, VulkanContext* context);
        ~VulkanGraphicsPipeline() override;

        vk::Pipeline GetPipeline() const { return m_Pipeline; }
        vk::PipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }

        OpaqueObject GetNativeType() override;
        void SetDebugName(const std::string& name) override;
        const GraphicsPipelineDesc& GetDesc() const override { return m_Desc; }
    private:
        VulkanContext* m_Context;
        GraphicsPipelineDesc m_Desc;

        vk::PipelineLayout m_PipelineLayout;
        vk::Pipeline m_Pipeline;
    };

    class VulkanComputePipeline : public ComputePipeline
    {
    public:
        VulkanComputePipeline(ComputePipelineDesc desc, VulkanContext* context);
        ~VulkanComputePipeline() override;

        vk::Pipeline GetPipeline() const { return m_Pipeline; }
        vk::PipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }

        OpaqueObject GetNativeType() override;
        void SetDebugName(const std::string& name) override;
        const ComputePipelineDesc& GetDesc() const override { return m_Desc; }

    private:
        VulkanContext* m_Context;
        ComputePipelineDesc m_Desc;

        vk::PipelineLayout m_PipelineLayout;
        vk::Pipeline m_Pipeline;
    };
}
