#include "VulkanBindingLayout.h"
#include "VulkanContext.h"

namespace qrhi::vulkan
{
    VulkanBindingLayout::VulkanBindingLayout(BindingLayoutDesc desc, VulkanContext* context)
        : desc(desc), isBindless(false), m_Context(context)
    {
        m_Context->GetLog()->Info("Creating BindingLayout");
        vk::ShaderStageFlags stages = ConvertShaderType(desc.shaderVisibility);

        for (const BindingLayoutItem& binding : desc.bindingItems)
        {
            if (binding.type == BindingResourceType::PushConstants)
                continue;

            vk::DescriptorType descriptorType = ConvertDescriptorType(binding.type);
            uint32_t descriptorCount = binding.size;

            vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = vk::DescriptorSetLayoutBinding()
                .setBinding(binding.slot)
                .setDescriptorType(descriptorType)
                .setDescriptorCount(descriptorCount)
                .setStageFlags(stages);

            vkLayoutBindings.push_back(descriptorSetLayoutBinding);
        }
    }

    VulkanBindingLayout::VulkanBindingLayout(BindlessLayoutDesc desc, VulkanContext* context)
        : bindlessDesc(desc), isBindless(true), m_Context(context)
    {
        m_Context->GetLog()->Info("Creating Bindless BindingLayout");
        vk::ShaderStageFlags stages = ConvertShaderType(desc.shaderVisibility);
        uint32_t bindingPoint = 0;
        uint32_t arraySize = desc.maxCapacity;

        // Immutable is assumed for now
        for (const BindingLayoutItem& item : desc.bindlessSlots)
        {
            vk::DescriptorType descriptorType = ConvertDescriptorType(item.type);

            if (item.type == BindingResourceType::DynamicUniformBuffer)
            {
                m_Context->GetLog()->Error("Dynamic Uniform Buffers are not supported with bindless layouts");
            }

            vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = vk::DescriptorSetLayoutBinding()
                .setBinding(bindingPoint)
                .setDescriptorType(descriptorType)
                .setDescriptorCount(arraySize)
                .setStageFlags(stages);

            vkLayoutBindings.push_back(descriptorSetLayoutBinding);
            bindingPoint++;
        }
    }

    VulkanBindingLayout::~VulkanBindingLayout()
    {
        if (descriptorSetLayout)
        {
            m_Context->GetDevice().destroyDescriptorSetLayout(descriptorSetLayout);
            descriptorSetLayout = vk::DescriptorSetLayout();
        }
    }

    Quest::OpaqueObject VulkanBindingLayout::GetNativeType()
    {
        Quest::OpaqueObject ret{};
        ret.pointer = static_cast<void*>(this);
        return ret;
    }

    void VulkanBindingLayout::SetDebugName(const std::string& name)
    {
        m_Context->SetDebugName(descriptorSetLayout, vk::ObjectType::eDescriptorSetLayout, name);
    }

    const BindingLayoutDesc* VulkanBindingLayout::GetDesc() const
    {
        return isBindless ? nullptr : &desc;
    }

    const BindlessLayoutDesc* VulkanBindingLayout::GetBindlessDesc() const
    {
        return isBindless ? &bindlessDesc : nullptr;
    }

    vk::Result VulkanBindingLayout::Bake()
    {
        auto descriptorSetLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
            .setBindingCount(vkLayoutBindings.size())
            .setPBindings(vkLayoutBindings.data());

        const vk::Result res = m_Context->GetDevice().createDescriptorSetLayout(&descriptorSetLayoutInfo, nullptr, &descriptorSetLayout);
        VK_CHECK(res);

        // count the number of descriptors required per type
        std::unordered_map<vk::DescriptorType, uint32_t> poolSizeMap;
        for (auto layoutBinding : vkLayoutBindings)
        {
            if (poolSizeMap.find(layoutBinding.descriptorType) == poolSizeMap.end())
            {
                poolSizeMap[layoutBinding.descriptorType] = 0;
            }

            poolSizeMap[layoutBinding.descriptorType] += layoutBinding.descriptorCount;
        }

        // compute descriptor pool size info
        for (auto poolSizeIter : poolSizeMap)
        {
            if (poolSizeIter.second > 0)
            {
                descriptorPoolSizeInfo.push_back(vk::DescriptorPoolSize()
                    .setType(poolSizeIter.first)
                    .setDescriptorCount(poolSizeIter.second));
            }
        }

        for (const auto poolsize : descriptorPoolSizeInfo)
        {
            std::string printstr = "Descriptor pool size: " + std::to_string(poolsize.descriptorCount);
            m_Context->GetLog()->Info(printstr);
        }

        return vk::Result::eSuccess;
    }
}
