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

        vk::Extent2D GetSwapchainExtent() const { return m_SwapchainExtent; }
        auto GetSwapchainImageFormat() const { return m_SwapchainFormat; }
        auto GetSwapchainImages() const { return m_SwapchainImages; }
        auto GetSwapchainImageViews() const { return m_SwapchainImageViews; }
        uint32_t GetSwapchainIndex() const { return m_SwapchainIndex; }

        // Signaled by the most recent AcquireNextImage(); the present-target submit waits on it.
        vk::Semaphore GetAcquireSemaphore() const { return m_AcquireSemaphores[m_AcquireSemaphoreIndex]; }
        // Signaled by the present-target submit; Present() waits on it.
        vk::Semaphore GetRenderFinishedSemaphore() const { return m_RenderSemaphores[m_SwapchainIndex]; }
    private:
        VulkanContext* m_Context;
        SwapchainDesc m_Desc;

        void CreateSwapchain();
        void DestroySwapchain();
        void RecreateSwapchain();

        vk::SwapchainKHR m_Swapchain;
        vk::Format m_SwapchainFormat;
        vk::Extent2D m_SwapchainExtent;
        std::vector<vk::Image> m_SwapchainImages;
        std::vector<vk::ImageView> m_SwapchainImageViews;
        uint32_t m_SwapchainIndex = 0;

        // Indexed by image -- the image index is known before submit/present.
        std::vector<vk::Semaphore> m_RenderSemaphores;
        // Ring indexed by acquire count -- the image index isn't known until after acquire.
        // Reuse is safe as long as frames in flight <= ring size (asserted by the Renderer).
        std::vector<vk::Semaphore> m_AcquireSemaphores;
        uint32_t m_AcquireSemaphoreIndex = 0;
    };
}