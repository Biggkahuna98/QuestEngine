#include "VulkanDevice.h"

#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include "VulkanShader.h"
#include "VulkanBindingLayout.h"
#include "VulkanBindingSet.h"
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

    BindingLayoutHandle VulkanDevice::CreateBindingLayout(BindingLayoutDesc desc)
    {
        VulkanBindingLayout* ret = new VulkanBindingLayout(desc, m_Context);
        ret->Bake();

        return BindingLayoutHandle(ret);
    }

    BindingLayoutHandle VulkanDevice::CreateBindlessLayout(BindlessLayoutDesc desc)
    {
        VulkanBindingLayout* ret = new VulkanBindingLayout(desc, m_Context);
        ret->Bake();

        return BindingLayoutHandle(ret);
    }

    BindingSetHandle VulkanDevice::CreateBindingSet(const BindingSetDesc& desc, BindingLayout* layout)
    {
        VulkanBindingLayout* bindingLayout = static_cast<VulkanBindingLayout*>(layout);

        VulkanBindingSet *set = new VulkanBindingSet(m_Context);
        set->desc = desc;
        set->layout = bindingLayout;

        vk::DescriptorSetLayout descriptorSetLayout = bindingLayout->descriptorSetLayout;
        std::vector<vk::DescriptorPoolSize> poolSizes = bindingLayout->descriptorPoolSizeInfo;

        auto poolInfo = vk::DescriptorPoolCreateInfo()
            .setPoolSizeCount(uint32_t(poolSizes.size()))
            .setPPoolSizes(poolSizes.data())
            .setMaxSets(1);
        vk::Result res = m_Context->GetDevice().createDescriptorPool(&poolInfo, nullptr, &set->descriptorPool);
        VK_CHECK(res);

        auto setCreateInfo = vk::DescriptorSetAllocateInfo()
            .setDescriptorPool(set->descriptorPool)
            .setDescriptorSetCount(1)
            .setPSetLayouts(&descriptorSetLayout);
        res = m_Context->GetDevice().allocateDescriptorSets(&setCreateInfo, &set->descriptorSet);
        VK_CHECK(res);

        std::vector<vk::DescriptorImageInfo> descriptorImageInfo;
        std::vector<vk::DescriptorBufferInfo> descriptorBufferInfo;
        std::vector<vk::WriteDescriptorSet> descriptorWriteInfo;
        descriptorImageInfo.reserve(desc.bindingSetItems.size());
        descriptorBufferInfo.reserve(desc.bindingSetItems.size());
        descriptorWriteInfo.reserve(desc.bindingSetItems.size());

        for (uint32_t bindingIndex = 0; bindingIndex < desc.bindingSetItems.size(); bindingIndex++)
        {
            const BindingSetItem& binding = desc.bindingSetItems[bindingIndex];

            if (binding.resourceHandle == nullptr)
                continue;

            // Keep the resource alive with a strong reference
            set->resources.push_back(binding.resourceHandle);

            vk::DescriptorType descriptorType = ConvertDescriptorType(binding.type);

            if (binding.type == BindingResourceType::Texture_SRV)
            {

            }

            if (binding.type == BindingResourceType::Texture_UAV)
            {

            }

            if (binding.type == BindingResourceType::StorageBuffer_SRV || binding.type == BindingResourceType::StorageBuffer_UAV)
            {

            }

            if (binding.type == BindingResourceType::UniformBuffer || binding.type == BindingResourceType::DynamicUniformBuffer)
            {
                const auto buffer = static_cast<VulkanBuffer*>(binding.resourceHandle);

                const auto range = binding.range.Resolve(buffer->GetDesc());

                auto& bufferInfo = descriptorBufferInfo.emplace_back();
                bufferInfo = vk::DescriptorBufferInfo()
                    .setBuffer(buffer->GetBuffer())
                    .setOffset(range.offsetInBytes)
                    .setRange(range.sizeInBytes);

                assert(buffer->GetBuffer());
                auto writeDescriptorData = GenerateWriteDescriptorData(binding.slot, binding.arrayIndex, descriptorType,
                    &set->descriptorSet, nullptr, &bufferInfo, nullptr);
                descriptorWriteInfo.push_back(writeDescriptorData);
            }

            if (binding.type == BindingResourceType::Sampler)
            {

            }

            if (binding.type == BindingResourceType::PushConstants)
            {
                continue;
            }
        }

        if (!descriptorWriteInfo.empty())
            m_Context->GetDevice().updateDescriptorSets(uint32_t(descriptorWriteInfo.size()), descriptorWriteInfo.data(), 0, nullptr);

        return BindingSetHandle(set);
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
        LOG_DEBUG("Buffer Mapped");
        VulkanBuffer* buff = static_cast<VulkanBuffer*>(buffer);
        void* mapping = nullptr;
        vmaMapMemory(m_Context->GetAllocator(), buff->allocation, &mapping);

        return mapping;
    }

    void VulkanDevice::UnmapBuffer(Buffer* buffer)
    {
        LOG_DEBUG("Buffer Unmapped");
        VulkanBuffer* buff = static_cast<VulkanBuffer*>(buffer);
        vmaUnmapMemory(m_Context->GetAllocator(), buff->allocation);
    }
}