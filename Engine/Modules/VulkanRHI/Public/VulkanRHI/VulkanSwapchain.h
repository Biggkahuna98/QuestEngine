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

        std::vector<vk::Semaphore> m_RenderSemaphores;
    };
}