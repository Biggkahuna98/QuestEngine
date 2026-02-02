#pragma once

#include "Vulkan_Backend.h"

namespace qrhi::vulkan
{
    class VulkanInputLayout : public qrhi::InputLayout
    {
    public:
        std::vector<VertexAttributeDesc> inputDescriptions;

        std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;

        uint32_t GetNumAttributes() const override;
        const VertexAttributeDesc* GetAttributeDesc(uint32_t index) const override;
    };
}