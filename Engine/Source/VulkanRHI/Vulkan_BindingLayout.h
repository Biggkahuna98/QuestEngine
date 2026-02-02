#pragma once

#include "Vulkan_Backend.h"
#include "Utility/StaticVector.h"

namespace qrhi::vulkan
{
    class VulkanBindingLayout : public qrhi::BindingLayout
    {
    public:
        BindingLayoutDesc desc;
        BindlessLayoutDesc bindlessDesc;
        bool isBindless;

        std::vector<vk::DescriptorSetLayoutBinding> vulkanLayoutBindings;

        vk::DescriptorSetLayout descriptorSetLayout;

        // Descriptor pool size info per binding set
        std::vector<vk::DescriptorPoolSize> descriptorPoolSizeInfo;

        VulkanBindingLayout(const VulkanContext& context, const BindingLayoutDesc& desc);
        VulkanBindingLayout(const VulkanContext& context, const BindlessLayoutDesc& desc);
        ~VulkanBindingLayout() override;
        const BindingLayoutDesc* GetDesc() const override { return isBindless ? nullptr : &desc; }
        const BindlessLayoutDesc* GetBindlessDesc() const override { return isBindless ? &bindlessDesc : nullptr; }

        // Generate the descriptor set layout
        vk::Result Bake();
    private:
        const VulkanContext& m_Context;
    };

    class BindingSet : public Quest::RefCounted<qrhi::BindingSet>
    {
    public:
        BindingSetDesc desc;
        BindingLayoutHandle layout;

        vk::DescriptorSet descriptorSet;

        std::vector<ResourceHandle> resources;

        BindingSet(const VulkanContext& context)
            : m_Context(context) {}
        ~BindingSet() override;

        const BindingSetDesc* GetDesc() const override { return &desc; }
        qrhi::BindingLayout* GetLayout() const override { return layout; }
    private:
        const VulkanContext& m_Context;
    };

    template <typename T>
    using BindingVector = Quest::StaticVector<T, c_MaxBindingLayouts>;
}