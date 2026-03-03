#include "VulkanDevice.h"

#include "VulkanContext.h"
#include "VulkanShader.h"

namespace qrhi::vulkan
{
    VulkanDevice::VulkanDevice(const VulkanContext* context)
        : m_Context(context)
    {
    }

    VulkanDevice::~VulkanDevice()
    {
    }

    BufferHandle VulkanDevice::CreateBuffer(BufferDesc desc)
    {
        return BufferHandle();
    }

    TextureHandle VulkanDevice::CreateTexture(TextureDesc desc)
    {
        return TextureHandle();
    }

    ShaderHandle VulkanDevice::CreateShader(ShaderDesc desc)
    {
        return Quest::RefCountPtr<VulkanShader>::Create(desc, m_Context);
    }

    GraphicsPipelineHandle VulkanDevice::CreateGraphicsPipeline(GraphicsPipelineDesc desc)
    {
        return GraphicsPipelineHandle();
    }

    ComputePipelineHandle VulkanDevice::CreateComputePipeline(ComputePipelineDesc desc)
    {
        return ComputePipelineHandle();
    }

    CommandListHandle VulkanDevice::CreateCommandList(CommandListDesc desc)
    {
        return CommandListHandle();
    }
}