#include "VulkanBindingSet.h"
#include "VulkanContext.h"

namespace qrhi::vulkan
{
    VulkanBindingSet::VulkanBindingSet(VulkanContext* context)
        : m_Context(context)
    {

    }

    VulkanBindingSet::~VulkanBindingSet()
    {
        if (descriptorPool)
        {
            m_Context->GetDevice().destroyDescriptorPool(descriptorPool, nullptr);
            descriptorPool = vk::DescriptorPool();
            descriptorSet = vk::DescriptorSet();
        }
    }

    void VulkanBindingSet::SetDebugName(const std::string& name)
    {
        m_Context->SetDebugName(descriptorSet, vk::ObjectType::eDescriptorSet, name);
    }
}
