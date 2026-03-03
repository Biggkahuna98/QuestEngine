#include "VulkanDevice.h"

#include "VulkanContext.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"
#include "VulkanCommandList.h"

namespace qrhi::vulkan
{
    VulkanDevice::VulkanDevice(VulkanContext* context)
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

    ShaderHandle VulkanDevice::CreateShaderMutliDesc(std::vector<ShaderDesc> descs)
    {
        return ShaderHandle();
    }

    GraphicsPipelineHandle VulkanDevice::CreateGraphicsPipeline(GraphicsPipelineDesc desc)
    {
        return Quest::RefCountPtr<VulkanGraphicsPipeline>::Create(desc, m_Context);
    }

    ComputePipelineHandle VulkanDevice::CreateComputePipeline(ComputePipelineDesc desc)
    {
        return Quest::RefCountPtr<VulkanComputePipeline>::Create(desc, m_Context);
    }

    CommandListHandle VulkanDevice::CreateCommandList(CommandListDesc desc)
    {
		return Quest::RefCountPtr<VulkanCommandList>::Create(desc, m_Context);
    }
}