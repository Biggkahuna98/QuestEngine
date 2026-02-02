#include "Vulkan_DeviceManager.h"

#include "VkBootstrap.h"
#include "VMA_Usage.h"

#include "Core/Window.h"
#include <GLFW/glfw3.h>

#include "Vulkan_Device.h"

// Vulkan hpp global dispatcher
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace qrhi::vulkan
{
    VulkanDeviceManager::VulkanDeviceManager(const DeviceDesc& desc)
    {
        QE_ASSERT(desc.messageCallback != nullptr);
        m_Context.messageCallback = desc.messageCallback;

        m_Context.info("Creating Vulkan Device Manager");
        m_Context.info("Initializing Vulkan...");

        // Initialize Vulkan - only one device is expected to exist at one time for now
        // Init the default hpp dispatcher
        VULKAN_HPP_DEFAULT_DISPATCHER.init();

        // Instance
        vkb::InstanceBuilder instance_builder;
        instance_builder
            .set_app_name("Quest Engine")
            .set_engine_name("Quest Engine")
            .request_validation_layers(true) // update to not required/toggleable
            .require_api_version(VK_API_VERSION_1_4);

        auto system_info_ret = vkb::SystemInfo::get_system_info();
        QE_ASSERT(system_info_ret);
        auto system_info = system_info_ret.value();

        QE_ASSERT(system_info.is_extension_available(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
        if (system_info.is_extension_available(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
            instance_builder.enable_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        instance_builder.set_debug_callback (
        [] (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void *pUserData)
            -> VkBool32 {
                (void)pUserData; // Mark pUserData as unused
                LOG_ERROR("[{}: {}] {}", string_VkDebugUtilsMessageSeverityFlagBitsEXT(messageSeverity), string_VkDebugUtilsMessageTypeFlagsEXT(messageType), pCallbackData->pMessage);
                return VK_FALSE;
            }
        );

        auto instance_ret = instance_builder.build();

        m_Context.instance = instance_ret->instance;
        m_Context.debugMessenger = instance_ret->debug_messenger;

        // Init dispatcher with instance
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Context.instance);

        QE_ASSERT(m_Context.instance);
        QE_ASSERT(m_Context.debugMessenger);

        // Create the surface
        int width, height;
        glfwGetFramebufferSize(static_cast<GLFWwindow*>(desc.window->GetNativeWindow()), &width, &height);
        m_WindowExtent= {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        // temp surface since glfw cant take the vulkan-hpp one
        VkSurfaceKHR tmpSurface;
        glfwCreateWindowSurface(m_Context.instance, static_cast<GLFWwindow*>(desc.window->GetNativeWindow()), nullptr, &tmpSurface);
        m_Context.surface = tmpSurface;

        QE_ASSERT(m_Context.surface);

        //vulkan 1.3 features
        VkPhysicalDeviceVulkan13Features features13{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        //features.dynamicRendering = true;
        features13.synchronization2 = true;
        features13.maintenance4 = true;

        //vulkan 1.2 features
        VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        features12.bufferDeviceAddress = true;
        features12.descriptorIndexing = true;

        // Physical device
        vkb::PhysicalDeviceSelector selector{ instance_ret.value() };
        vkb::PhysicalDevice physicalDevice = selector
            .set_minimum_version(1, 4)
            .set_required_features_13(features13)
            .set_required_features_12(features12)
            .set_surface(m_Context.surface)
            .select()
            .value();

        vkb::DeviceBuilder device_builder{physicalDevice};
        vkb::Device device = device_builder.build().value();

        m_Context.physicalDevice = physicalDevice;
        m_Context.device = device.device;

        QE_ASSERT(m_Context.physicalDevice);
        QE_ASSERT(m_Context.device);

        // Init dispatcher with device
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Context.device);

        // VMA Allocator
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = m_Context.physicalDevice;
        allocatorInfo.device = m_Context.device;
        allocatorInfo.instance = m_Context.instance;
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        //allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;

        VmaAllocator tmpAllocator;
        VK_CHECK_OLD(vmaCreateAllocator(&allocatorInfo, &tmpAllocator));
        m_Context.allocator = tmpAllocator;

        QE_ASSERT(m_Context.allocator);

        // Get the queues, for now we will assume all queues are being grabbed
        m_Context.queues[static_cast<uint32_t>(QueueType::Graphics)] = std::make_unique<Queue>(m_Context, QueueType::Graphics,
            device.get_queue(vkb::QueueType::graphics).value(),
            device.get_queue_index(vkb::QueueType::graphics).value());
        m_Context.queues[static_cast<uint32_t>(QueueType::Compute)] = std::make_unique<Queue>(m_Context, QueueType::Compute,
            device.get_queue(vkb::QueueType::compute).value(),
            device.get_queue_index(vkb::QueueType::compute).value());
        m_Context.queues[static_cast<uint32_t>(QueueType::Copy)] = std::make_unique<Queue>(m_Context, QueueType::Copy,
            device.get_queue(vkb::QueueType::transfer).value(),
            device.get_queue_index(vkb::QueueType::transfer).value());
        m_Context.queues[static_cast<uint32_t>(QueueType::Present)] = std::make_unique<Queue>(m_Context, QueueType::Count,
                device.get_queue(vkb::QueueType::present).value(),
                device.get_queue_index(vkb::QueueType::present).value());


        QE_ASSERT(m_Context.queues[static_cast<uint32_t>(QueueType::Graphics)]);
        QE_ASSERT(m_Context.queues[static_cast<uint32_t>(QueueType::Compute)]);
        QE_ASSERT(m_Context.queues[static_cast<uint32_t>(QueueType::Copy)]);
        QE_ASSERT(m_Context.queues[static_cast<uint32_t>(QueueType::Present)]);

        // Create swapchain
        CreateSwapchain();

        // Create the qrhi::Device
        m_Device = std::make_shared<VulkanDevice>(desc, m_Context);
    }

    VulkanDeviceManager::~VulkanDeviceManager()
    {
    }

    GraphicsAPI VulkanDeviceManager::GetGraphicsAPI() const
    {
        return GraphicsAPI::Vulkan;
    }

    void VulkanDeviceManager::BeginFrame()
    {
        //LOG_INFO("Vulkan::BeginFrame");
        /*const auto& semaphore = m_AcquireSemaphores[m_AcquireSemaphoreIndex];

        vk::Result res;

        res = m_Context.device.acquireNextImageKHR(m_Swapchain, std::numeric_limits<uint64_t>::max(),
            semaphore, vk::Fence(), &m_SwapchainIndex);

        QE_ASSERT(res == vk::Result::eSuccess);

        m_AcquireSemaphoreIndex = (m_AcquireSemaphoreIndex + 1) % m_AcquireSemaphores.size();

        VulkanDevice* vd = static_cast<VulkanDevice*>(m_Device.get());
        // Schedule the wait, add it to the next submitted command list
        vd->QueueWaitForSemaphore(QueueType::Graphics, semaphore, 0);*/
    }

    void VulkanDeviceManager::EndFrame()
    {
        //LOG_INFO("Vulkan::EndFrame");
    }

    void VulkanDeviceManager::PresentFrame()
    {
        //LOG_INFO("Vulkan::PresentFrame");
        /*const auto& semaphore = m_PresentSemaphores[m_SwapchainIndex];
        VulkanDevice* vd = static_cast<VulkanDevice*>(m_Device.get());*/

    }

    void VulkanDeviceManager::Shutdown()
    {
        // Wait idle
        m_Context.device.waitIdle();

        DestroySwapchain();

        m_Context.instance.destroySurfaceKHR(m_Context.surface);

        vmaDestroyAllocator(m_Context.allocator);

        m_Context.device.destroy();

        m_Context.instance.destroyDebugUtilsMessengerEXT(m_Context.debugMessenger);
        m_Context.instance.destroy();
    }

    void VulkanDeviceManager::CreateSwapchain()
    {
        m_Context.info("Creating Swapchain");

        vkb::SwapchainBuilder swapchain_builder{m_Context.physicalDevice, m_Context.device, m_Context.surface};
        m_SwapchainFormat = vk::Format::eB8G8R8A8Unorm;
        vkb::Swapchain swapchain = swapchain_builder
            .set_desired_format({.format = static_cast<VkFormat>(m_SwapchainFormat),
                                    .colorSpace = static_cast<VkColorSpaceKHR>(vk::ColorSpaceKHR::eSrgbNonlinear)})
            .set_desired_present_mode(static_cast<VkPresentModeKHR>(vk::PresentModeKHR::eFifo))
            .set_desired_extent(m_WindowExtent.width, m_WindowExtent.height)
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
            VK_CHECK(m_Context.device.createSemaphore(&semaphoreCreateInfo, nullptr, &m_RenderSemaphores[i]));
        }
    }

    void VulkanDeviceManager::DestroySwapchain()
    {
        m_Context.info("Destroying Swapchain");

        for (int i = 0; i < m_RenderSemaphores.size(); i++)
        {
            m_Context.device.destroySemaphore(m_RenderSemaphores[i], nullptr);
        }

        for (int i = 0; i < m_SwapchainImageViews.size(); i++)
        {
            m_Context.device.destroyImageView(m_SwapchainImageViews[i], nullptr);
        }

        m_Context.device.destroySwapchainKHR(m_Swapchain, nullptr);
    }

    FrameData CreateFrameData(const VulkanContext& context)
    {
        FrameData frameData;

        vk::SemaphoreCreateInfo semaphoreCreateInfo{};
        vk::FenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

        VK_CHECK(context.device.createFence(&fenceCreateInfo, nullptr, &frameData.PresentFence));
        VK_CHECK(context.device.createSemaphore(&semaphoreCreateInfo, nullptr, &frameData.PresentSemaphore));

        return frameData;
    }

    void DestroyFrameData(const VulkanContext& context, FrameData& frameData)
    {
        context.device.destroyFence(frameData.PresentFence, nullptr);
        context.device.destroySemaphore(frameData.PresentSemaphore, nullptr);
    }
}
