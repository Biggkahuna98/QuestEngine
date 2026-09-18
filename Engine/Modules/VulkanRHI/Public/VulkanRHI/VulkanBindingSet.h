#pragma once

#include "Core/RHI/ShaderBindings.h"
#include "VulkanCommon.h"

namespace Quest::RHI::Vulkan
{
    class VulkanBindingSet : public BindingSet
    {
    public:
        VulkanBindingSet(VulkanContext* context);
        ~VulkanBindingSet() override;
        const BindingSetDesc* GetDesc() const override { return &desc; }
        BindingLayout* GetLayout() override { return layout.Get(); }
        OpaqueObject GetNativeType() override { return static_cast<void*>(this); }
        void SetDebugName(const std::string& name) override;

        BindingSetDesc desc;
        BindingLayoutHandle layout;

        vk::DescriptorSet descriptorSet;
        vk::DescriptorPool descriptorPool;
        std::vector<ResourceHandle> resources;
        StaticVector<Buffer*, c_MaxDynamicUniformBuffersPerLayout> dynamicUniformBuffers;
        
    private:
        VulkanContext* m_Context;
    };
}