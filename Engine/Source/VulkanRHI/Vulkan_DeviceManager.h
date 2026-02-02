#pragma once
#include "RHI/DeviceManager.h"
#include "Vulkan_Backend.h"

namespace qrhi::vulkan
{
    // Per-frame storage
    struct FrameData
    {
        vk::Semaphore PresentSemaphore;
        vk::Fence PresentFence; // maybe
    };

    class VulkanDeviceManager : public qrhi::DeviceManager
    {
    public:
        VulkanDeviceManager(const DeviceDesc& desc);
        ~VulkanDeviceManager() override;

        GraphicsAPI GetGraphicsAPI() const override;

        void BeginFrame() override;
        void EndFrame() override;
        void PresentFrame() override;

        void Shutdown() override;
    private:
        VulkanContext m_Context;

        vk::SwapchainKHR m_Swapchain;
        vk::Format m_SwapchainFormat;
        std::vector<vk::Image> m_SwapchainImages;
        std::vector<vk::ImageView> m_SwapchainImageViews;
        uint32_t m_SwapchainIndex = 0;
        vk::Extent2D m_SwapchainExtent;
        vk::Extent2D m_WindowExtent;

        std::vector<vk::Semaphore> m_RenderSemaphores; // size of swapchain images

        std::vector<FrameData> m_FrameData;
        uint32_t m_FrameNumber = 0;

        //std::vector<vk::Semaphore> m_AcquireSemaphores;
        //std::vector<vk::Semaphore> m_PresentSemaphores;
        //uint32_t m_AcquireSemaphoreIndex = 0;

        vk::DescriptorPool m_DescriptorPool;

        void CreateSwapchain();
        void DestroySwapchain();
    };

    FrameData CreateFrameData(const VulkanContext& context);
    void DestroyFrameData(const VulkanContext& context, FrameData& frameData);
}