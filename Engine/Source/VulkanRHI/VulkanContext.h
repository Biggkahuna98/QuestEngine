#pragma once

#include "RHI/Context.h"
#include "VulkanCommon.h"
#include "VMA_Usage.h"
#include "Utility/StaticVector.h"
#include <array>

namespace qrhi::vulkan
{
    class VulkanDevice;

    struct FrameData
    {
        vk::Semaphore presentCompleteSemaphore;
        vk::Fence inFlightFence;
    };

    class VulkanContext : public Context
    {
    public:
        VulkanContext(ContextDesc desc);
        ~VulkanContext() override;

        DeviceHandle CreateDevice() override;

        void BeginFrame() override;
        void EndFrame() override;
        void PresentFrame() override;

        void Shutdown() override;

        // VulkanContext specific functions
        void SetDebugName(const void* handle, const vk::ObjectType objType, const std::string_view& name) const;
        MessageCallback* GetLog() const { return m_Log; }
        void logInfo(std::string_view message) const { m_Log->Info(message); }
        void logWarning(std::string_view message) const { m_Log->Warning(message); }
        void logError(std::string_view message) const { m_Log->Error(message); }

        vk::Instance GetInstance() const { return m_Instance; }
        vk::SurfaceKHR GetSurface() const { return m_Surface; }
        vk::Extent2D GetWindowExtent() const { return m_WindowExtent; }
        vk::Device GetDevice() const { return m_Device; }
        vk::PhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
        vk::Extent2D GetSwapchainExtent() const { return m_SwapchainExtent; }
        auto GetSwapchainImageFormat() const { return m_SwapchainFormat; }
        auto GetSwapchainImages() const { return m_SwapchainImages; }
        auto GetSwapchainImageViews() const { return m_SwapchainImageViews; }
        uint32_t GetSwapchainIndex() const { return m_SwapchainIndex; }
        VmaAllocator GetAllocator() const { return m_Allocator; }
        Queue* GetQueue(QueueType type) const;
        FrameData& GetFrameData();
        uint32_t GetFrameIndex() const { return m_FrameCount % static_cast<uint32_t>(m_Desc.framesInFlight); }
        auto GetDynamicStates() const { return m_DynamicStates; }

        // Creation helpers
        void CreateSwapchain();
        void DestroySwapchain() const;
        void RecreateSwapchain();

        // Idk where to put this, so here will do for now
        void TransitionImage(vk::CommandBuffer cmdBuffer, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const;
    private:
        ContextDesc m_Desc;
        MessageCallback* m_Log;

        // Vulkan objects
        vk::Instance m_Instance;
        vk::DebugUtilsMessengerEXT m_DebugMessenger;
        vk::Extent2D m_WindowExtent;
        vk::SurfaceKHR m_Surface;

        vk::PhysicalDevice m_PhysicalDevice;
        vk::Device m_Device;
        VmaAllocator m_Allocator;

        vk::SwapchainKHR m_Swapchain;
        vk::Format m_SwapchainFormat;
        vk::Extent2D m_SwapchainExtent;
        std::vector<vk::Image> m_SwapchainImages;
        std::vector<vk::ImageView> m_SwapchainImageViews;
        uint32_t m_SwapchainIndex = 0;

        std::vector<vk::Semaphore> m_RenderSemaphores;

        Quest::StaticVector<FrameData, static_cast<int>(FramesInFlight::Count)> m_FrameData;
        uint64_t m_FrameCount = 0;

        // Hardware queues
        std::array<std::unique_ptr<Queue>, static_cast<uint32_t>(QueueType::Count)> m_Queues;

        // Dynamic State for now
        std::vector<vk::DynamicState> m_DynamicStates;
    };
}
