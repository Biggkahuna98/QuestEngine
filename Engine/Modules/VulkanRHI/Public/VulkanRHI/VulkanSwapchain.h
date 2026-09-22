#pragma once

#include "Core/RHI/Swapchain.h"
#include "VulkanCommon.h"
#include "VulkanContext.h"

namespace Quest::Vulkan
{
    class VulkanSwapchain : public Swapchain
    {
    public:
        VulkanSwapchain(VulkanContext* ctx, SwapchainDesc desc);
        ~VulkanSwapchain() override;

        uint32_t AcquireNextImage() override;
        TextureHandle GetCurrentBackBuffer() override;
        uint32_t GetCurrentImageIndex() const override;
        uint32_t GetImageCount() const override;
        void Present() override;
        void Resize(uint32_t width, uint32_t height) override;
        Format GetFormat() const override;
    private:
        VulkanContext* m_Context;
        SwapchainDesc m_Desc;
    };
}