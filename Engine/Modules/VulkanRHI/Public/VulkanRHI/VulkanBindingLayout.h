#pragma once

#include "VulkanCommon.h"
#include "Core/RHI/ShaderBindings.h"

namespace Quest::Vulkan
{
    class VulkanBindingLayout : public BindingLayout
    {
    public:
        VulkanBindingLayout(BindingLayoutDesc desc, VulkanContext* context);
        VulkanBindingLayout(BindlessLayoutDesc desc, VulkanContext* context);

        ~VulkanBindingLayout() override;

        OpaqueObject GetNativeType() override;
        void SetDebugName(const std::string& name) override;
        const BindingLayoutDesc* GetDesc() const override;
        const BindlessLayoutDesc* GetBindlessDesc() const override;

        vk::Result Bake();

        BindingLayoutDesc desc;
        BindlessLayoutDesc bindlessDesc;
        bool isBindless;

        std::vector<vk::DescriptorSetLayoutBinding> vkLayoutBindings;
        vk::DescriptorSetLayout descriptorSetLayout;

        // descriptor pool size information per binding set
        std::vector<vk::DescriptorPoolSize> descriptorPoolSizeInfo;
    private:
        VulkanContext* m_Context;
    };
}