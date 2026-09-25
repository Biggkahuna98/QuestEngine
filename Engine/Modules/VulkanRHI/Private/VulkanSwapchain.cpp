#include "VulkanRHI/VulkanSwapchain.h"
#include "VulkanRHI/VkBootstrap.h"

namespace Quest::Vulkan
{
    VulkanSwapchain::VulkanSwapchain(VulkanContext* ctx, SwapchainDesc desc)
        : m_Context(ctx), m_Desc(desc)
    {
        CreateSwapchain();
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        DestroySwapchain();
    }

    uint32_t VulkanSwapchain::AcquireNextImage()
    {
        auto [result, imageIndex] = m_Context->GetDevice().acquireNextImageKHR(m_Swapchain, std::numeric_limits<uint64_t>::max(), frameData.presentCompleteSemaphore, nullptr);
        VK_CHECK(result);
        m_SwapchainIndex = imageIndex;

        return m_SwapchainIndex;
    }

    TextureHandle VulkanSwapchain::GetCurrentBackBuffer()
    {
        return TextureHandle();
    }

    uint32_t VulkanSwapchain::GetCurrentImageIndex() const
    {
        return m_SwapchainIndex;
    }

    uint32_t VulkanSwapchain::GetImageCount() const
    {
        return m_SwapchainImages.size();
    }

    void VulkanSwapchain::Present()
    {
        vk::Semaphore renderFinishedSemaphore = m_RenderSemaphores[m_SwapchainIndex];
        vk::PresentInfoKHR presentInfoKHR{
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &renderFinishedSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &m_Swapchain,
            .pImageIndices = &m_SwapchainIndex};

        vk::Result result = m_Context->GetQueue(Quest::QueueType::Present)->GetQueue().presentKHR(presentInfoKHR);
    }

    void VulkanSwapchain::Resize(uint32_t width, uint32_t height)
    {
        m_SwapchainExtent.width = width;
        m_SwapchainExtent.height = height;
        RecreateSwapchain();
    }

    Format VulkanSwapchain::GetFormat() const
    {
        return m_Desc.format;
    }

    void VulkanSwapchain::CreateSwapchain()
    {
        vk::Device device = m_Context->GetDevice();
        vk::PhysicalDevice pdevice = m_Context->GetPhysicalDevice();
        vk::SurfaceKHR surface = m_Context->GetSurface();
        vk::Extent2D windowExtent = m_Context->GetWindowExtent();

        vkb::SwapchainBuilder swapchain_builder{pdevice, device, surface};
        m_SwapchainFormat = vk::Format::eB8G8R8A8Unorm;
        vkb::Swapchain swapchain = swapchain_builder
            .set_desired_format({.format = static_cast<VkFormat>(m_SwapchainFormat),
                                    .colorSpace = static_cast<VkColorSpaceKHR>(vk::ColorSpaceKHR::eSrgbNonlinear)})
            .set_desired_present_mode(static_cast<VkPresentModeKHR>(vk::PresentModeKHR::eFifo))
            .set_desired_extent(windowExtent.width, windowExtent.height)
            .add_image_usage_flags(static_cast<VkImageUsageFlags>(vk::ImageUsageFlagBits::eTransferDst))
            .build()
            .value();

        m_Swapchain = swapchain.swapchain;
        m_SwapchainExtent = swapchain.extent;
        auto images = swapchain.get_images().value();
        auto imageViews = swapchain.get_image_views().value();
        m_SwapchainImages = std::vector<vk::Image>(images.begin(), images.end());
        m_SwapchainImageViews = std::vector<vk::ImageView>(imageViews.begin(), imageViews.end());

        m_RenderSemaphores.resize(m_SwapchainImages.size());
        for (int i = 0; i < m_RenderSemaphores.size(); i++)
        {
            vk::SemaphoreCreateInfo semaphoreCreateInfo{};
            VK_CHECK(device.createSemaphore(&semaphoreCreateInfo, nullptr, &m_RenderSemaphores[i]));
            m_Context->SetDebugName(m_RenderSemaphores[i], vk::ObjectType::eSemaphore, "renderSemaphore: " + std::to_string(i));
        }
    }

    void VulkanSwapchain::DestroySwapchain()
    {
        vk::Device device = m_Context->GetDevice();

        for (int i = 0; i < m_RenderSemaphores.size(); i++)
        {
            device.destroySemaphore(m_RenderSemaphores[i], nullptr);
        }

        for (int i = 0; i < m_SwapchainImageViews.size(); i++)
        {
            device.destroyImageView(m_SwapchainImageViews[i], nullptr);
        }

        device.destroySwapchainKHR(m_Swapchain, nullptr);
    }

    void VulkanSwapchain::RecreateSwapchain()
    {
        DestroySwapchain();
        CreateSwapchain();
    }
}
