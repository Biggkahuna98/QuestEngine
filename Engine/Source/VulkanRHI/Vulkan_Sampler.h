#pragma once

#include "Vulkan_Backend.h"

namespace qrhi::vulkan
{
    class VulkanSampler : public qrhi::Sampler
    {
    public:
        SamplerDesc desc;

        vk::SamplerCreateInfo samplerCreateInfo;
        vk::Sampler sampler;

        VulkanSampler(const VulkanContext& context)
            : m_Context(context) {}

        const SamplerDesc& GetDesc() const override { return desc; }
    private:
        const VulkanContext& m_Context;
    };
}