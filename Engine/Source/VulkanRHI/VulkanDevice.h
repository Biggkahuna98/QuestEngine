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
        BindingLayoutHandle CreateBindingLayout(BindingLayoutDesc desc) override;
        BindingLayoutHandle CreateBindlessLayout(BindlessLayoutDesc desc) override;
        BindingSetHandle CreateBindingSet(const BindingSetDesc& desc, BindingLayout* layout) override;
        GraphicsPipelineHandle CreateGraphicsPipeline(GraphicsPipelineDesc desc) override;
        ComputePipelineHandle CreateComputePipeline(ComputePipelineDesc desc) override;
        CommandListHandle CreateCommandList(CommandListDesc desc) override;

        void* MapBuffer(Buffer* buffer) override;
        void UnmapBuffer(Buffer* buffer) override;
    private:
        VulkanContext* m_Context;
    };
}