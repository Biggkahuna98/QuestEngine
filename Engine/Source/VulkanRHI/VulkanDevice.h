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
        VulkanDevice(const VulkanContext* context);
        ~VulkanDevice() override;

        BufferHandle CreateBuffer(BufferDesc desc) override;
        TextureHandle CreateTexture(TextureDesc desc) override;
        ShaderHandle CreateShader(ShaderDesc desc) override;
        GraphicsPipelineHandle CreateGraphicsPipeline(GraphicsPipelineDesc desc) override;
        ComputePipelineHandle CreateComputePipeline(ComputePipelineDesc desc) override;
        CommandListHandle CreateCommandList(CommandListDesc desc) override;
    private:
        const VulkanContext* m_Context;
    };
}