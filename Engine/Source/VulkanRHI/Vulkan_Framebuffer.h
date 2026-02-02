#pragma once

#include "Vulkan_Backend.h"

namespace qrhi::vulkan
{
    class VulkanFramebuffer : public qrhi::Framebuffer
    {
    public:
        FramebufferDesc desc;
        FramebufferInfo framebufferInfo;

        // Might be moot or just an image since I will be using dynamic rendering
        vk::Framebuffer framebuffer;

        std::vector<ResourceHandle> resources;

        VulkanFramebuffer(const VulkanContext& context)
            : m_Context(context) {}
        ~VulkanFramebuffer() override;

        const FramebufferDesc& GetDesc() const override { return desc; }
        const FramebufferInfo& GetInfo() const override { return framebufferInfo; }
    private:
        const VulkanContext& m_Context;
    };
}