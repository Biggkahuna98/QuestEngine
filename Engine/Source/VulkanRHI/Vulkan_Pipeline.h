#pragma once

#include "Vulkan_Backend.h"
#include "Vulkan_BindingLayout.h"

namespace qrhi::vulkan
{
    vk::Result CreatePipelineLayout(
        vk::PipelineLayout& outPipelineLayout,
        BindingVector<Quest::RefCountPtr<BindingLayout>>& outBindingLayouts,
        vk::ShaderStageFlags& outPushConstantVisibility,
        BindingVector<uint32_t>& outStateBindingIndexToPipelineBindingIndex,
        VulkanContext& context,
        BindingLayoutVector& inBindingLayouts);

    class VulkanGraphicsPipeline : public qrhi::GraphicsPipeline
    {
    public:
        GraphicsPipelineDesc desc;
        FramebufferInfo framebufferInfo;
        ShaderType shaderMask = ShaderType::None;
        BindingVector<Quest::RefCountPtr<BindingLayout>> pipelineBindingLayouts;
        BindingVector<uint32_t> descriptorSetIndexToBindingIndex;
        vk::PipelineLayout pipelineLayout;
        vk::Pipeline pipeline;
        vk::ShaderStageFlags pushConstantVisibility;

        VulkanGraphicsPipeline(const VulkanContext& context)
            : m_Context(context) {}
        ~VulkanGraphicsPipeline() override;

        const GraphicsPipelineDesc& GetDesc() const override { return desc; }
        const FramebufferInfo& GetFramebufferInfo() const override { return framebufferInfo; }
    private:
        const VulkanContext& m_Context;
    };

    class VulkanComputePipeline : public qrhi::ComputePipeline
    {
    public:
        ComputePipelineDesc desc;

        BindingVector<Quest::RefCountPtr<BindingLayout>> pipelineBindingLayouts;
        BindingVector<uint32_t> descriptorSetIndexToBindingIndex;
        vk::PipelineLayout pipelineLayout;
        vk::Pipeline pipeline;
        vk::ShaderStageFlags pushConstantVisibility;

        VulkanComputePipeline(const VulkanContext& context)
            : m_Context(context) {}
        ~VulkanComputePipeline() override;

        const ComputePipelineDesc& GetDesc() const override { return desc; }
    private:
        const VulkanContext& m_Context;
    };
}