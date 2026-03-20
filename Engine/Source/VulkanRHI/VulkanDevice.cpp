#include "VulkanDevice.h"

#include "VulkanBuffer.h"
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
        return Quest::RefCountPtr<VulkanBuffer>::Create(desc, m_Context);;
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

    void* VulkanDevice::MapBuffer(Buffer* buffer)
    {
        VulkanBuffer* buff = static_cast<VulkanBuffer*>(buffer);
        void* mapping = nullptr;
        vmaMapMemory(m_Context->GetAllocator(), buff->allocation, &mapping);

        return mapping;
    }

    void VulkanDevice::UnmapBuffer(Buffer* buffer)
    {
        VulkanBuffer* buff = static_cast<VulkanBuffer*>(buffer);
        vmaUnmapMemory(m_Context->GetAllocator(), buff->allocation);
    }
}