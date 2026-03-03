#pragma once

#include "RHI/Device.h"
#include "VulkanCommon.h"

// Resources
#include "VulkanShader.h"

namespace qrhi::vulkan
{
    class VulkanContext;

    class VulkanDevice : public Device
    {
    public:
        VulkanDevice(VulkanContext* context);
        ~VulkanDevice() override;

        BufferHandle CreateBuffer(BufferDesc desc) override;
        TextureHandle CreateTexture(TextureDesc desc) override;
        ShaderHandle CreateShader(ShaderDesc desc) override;
        ShaderHandle CreateShaderMutliDesc(std::vector<ShaderDesc> descs) override;
        GraphicsPipelineHandle CreateGraphicsPipeline(GraphicsPipelineDesc desc) override;
        ComputePipelineHandle CreateComputePipeline(ComputePipelineDesc desc) override;
        CommandListHandle CreateCommandList(CommandListDesc desc) override;
    private:
        VulkanContext* m_Context;
    };
}