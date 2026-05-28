#pragma once

#include "RHI/QRHI.h"
#include "VulkanCommon.h"

namespace qrhi::vulkan
{
    class VulkanBindingSet : public BindingSet
    {
    public:
        VulkanBindingSet(VulkanContext* context);
        ~VulkanBindingSet() override;
        const BindingSetDesc* GetDesc() const override { return &desc; }
        BindingLayout* GetLayout() override { return layout.Get(); }
        Quest::OpaqueObject GetNativeType() override { return static_cast<void*>(this); }
        void SetDebugName(const std::string& name) override;

        BindingSetDesc desc;
        BindingLayoutHandle layout;

        vk::DescriptorSet descriptorSet;
        vk::DescriptorPool descriptorPool;
        std::vector<ResourceHandle> resources;
        Quest::StaticVector<Buffer*, c_MaxDynamicUniformBuffersPerLayout> dynamicUniformBuffers;
        
    private:
        VulkanContext* m_Context;
    };
}