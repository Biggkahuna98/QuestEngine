#include "VulkanRHI/VulkanSwapchain.h"

namespace Quest::Vulkan
{
    VulkanSwapchain::VulkanSwapchain(VulkanContext* ctx, SwapchainDesc desc)
        : m_Context(ctx), m_Desc(desc)
    {
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
    }

    uint32_t VulkanSwapchain::AcquireNextImage()
    {
    }

    TextureHandle VulkanSwapchain::GetCurrentBackBuffer()
    {
        return TextureHandle();
    }

    uint32_t VulkanSwapchain::GetCurrentImageIndex() const
    {
    }

    uint32_t VulkanSwapchain::GetImageCount() const
    {
    }

    void VulkanSwapchain::Present()
    {
    }

    void VulkanSwapchain::Resize(uint32_t width, uint32_t height)
    {
    }

    Format VulkanSwapchain::GetFormat() const
    {
        return m_Desc.format;
    }
}
