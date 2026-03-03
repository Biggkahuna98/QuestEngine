#include "VulkanContext.h"
#include "VulkanDevice.h"

#include "VkBootstrap.h"
#include "Core/Core.h"
#include <GLFW/glfw3.h>

#include "RHI/ShaderUtils.h"

// Vulkan hpp global dispatcher
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace qrhi::vulkan
{
    VulkanContext::VulkanContext(ContextDesc desc)
        : m_Desc(desc), m_Log(desc.messageCallback)
    {
        QE_ASSERT(desc.messageCallback != nullptr);
        m_Log->Info("Creating Vulkan Context");

        VULKAN_HPP_DEFAULT_DISPATCHER.init();

        vkb::InstanceBuilder instance_builder;
        instance_builder
            .set_app_name("Quest Engine")
            .set_engine_name("Quest Engine")
            .request_validation_layers(desc.enableAPISpecificValidation)
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
                LOG_ERROR("[{}: {}] {}", vkb::to_string_message_severity(messageSeverity), vkb::to_string_message_type(messageType), pCallbackData->pMessage);
                return VK_FALSE;
            }
        );

        auto instance_ret = instance_builder.build();
        m_Instance = instance_ret->instance;
        m_DebugMessenger = instance_ret->debug_messenger;
        QE_ASSERT(m_Instance);
        QE_ASSERT(m_DebugMessenger);

        // Init the dispatcher with the instance
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Instance);

        // Create the surface
        int width, height;
        glfwGetFramebufferSize(static_cast<GLFWwindow*>(desc.window->GetNativeWindow()), &width, &height);
        m_WindowExtent= {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        // temp surface since glfw cant take the vulkan-hpp one
        VkSurfaceKHR tmpSurface;
        glfwCreateWindowSurface(m_Instance, static_cast<GLFWwindow*>(desc.window->GetNativeWindow()), nullptr, &tmpSurface);
        m_Surface = tmpSurface;
        QE_ASSERT(m_Surface);

        // Physical device/logical device

        VkPhysicalDeviceVulkan14Features features14{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES };
        features14.dynamicRenderingLocalRead = true;
        features14.pushDescriptor = true;
        features14.maintenance5 = true;
        features14.maintenance6 = true;

        //vulkan 1.3 features
        VkPhysicalDeviceVulkan13Features features13{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        features13.dynamicRendering = true;
        features13.synchronization2 = true;
        features13.maintenance4 = true;

        //vulkan 1.2 features
        VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        features12.bufferDeviceAddress = true;
        features12.descriptorIndexing = true;
        features12.timelineSemaphore = true;

        // vulkan 1.1 features
        VkPhysicalDeviceVulkan11Features features11{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
        features11.shaderDrawParameters = true;

        // Physical device
        vkb::PhysicalDeviceSelector selector{instance_ret.value()};
        vkb::PhysicalDevice physicalDevice = selector
            .set_minimum_version(1, 4)
            .set_required_features_14(features14)
            .set_required_features_13(features13)
            .set_required_features_12(features12)
            .set_required_features_11(features11)
            .set_surface(m_Surface)
            .select()
            .value();

        vkb::DeviceBuilder device_builder{physicalDevice};
        vkb::Device device = device_builder.build().value();

        m_PhysicalDevice = physicalDevice;
        m_Device = device;

        QE_ASSERT(m_PhysicalDevice);
        QE_ASSERT(m_Device);

        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Device);

        // VMA Allocator
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = m_PhysicalDevice;
        allocatorInfo.device = m_Device;
        allocatorInfo.instance = m_Instance;
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        //allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;

        VmaAllocator tmpAllocator;
        VK_CHECK_OLD(vmaCreateAllocator(&allocatorInfo, &tmpAllocator));
        m_Allocator = tmpAllocator;

        QE_ASSERT(m_Allocator);

        // Assume all queues exist for now
        m_Queues[static_cast<uint32_t>(QueueType::Graphics)] = std::make_unique<Queue>(this, QueueType::Graphics,
            device.get_queue(vkb::QueueType::graphics).value(), device.get_queue_index(vkb::QueueType::graphics).value());
        m_Queues[static_cast<uint32_t>(QueueType::Present)] = std::make_unique<Queue>(this, QueueType::Present,
            device.get_queue(vkb::QueueType::present).value(), device.get_queue_index(vkb::QueueType::present).value());
        m_Queues[static_cast<uint32_t>(QueueType::Compute)] = std::make_unique<Queue>(this, QueueType::Compute,
            device.get_queue(vkb::QueueType::compute).value(), device.get_queue_index(vkb::QueueType::compute).value());
        m_Queues[static_cast<uint32_t>(QueueType::Transfer)] = std::make_unique<Queue>(this, QueueType::Transfer,
            device.get_queue(vkb::QueueType::transfer).value(), device.get_queue_index(vkb::QueueType::transfer).value());

        QE_ASSERT(m_Queues[static_cast<uint32_t>(QueueType::Graphics)]);
        QE_ASSERT(m_Queues[static_cast<uint32_t>(QueueType::Present)]);
        QE_ASSERT(m_Queues[static_cast<uint32_t>(QueueType::Compute)]);
        QE_ASSERT(m_Queues[static_cast<uint32_t>(QueueType::Transfer)]);

        CreateSwapchain();

        // Push framedata
        for (int i = 0; i < static_cast<int>(FramesInFlight::Count); i++)
        {
            LOG_DEBUG("Creating frame data {}", i);
            FrameData fd{};
            std::string semName = "presentCompleteSemaphore: " + std::to_string(i);
            std::string fenceName = "inFlightFence: " + std::to_string(i);
            fd.presentCompleteSemaphore = m_Device.createSemaphore(vk::SemaphoreCreateInfo());
            fd.inFlightFence = m_Device.createFence(vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled });
            SetDebugName(fd.presentCompleteSemaphore, vk::ObjectType::eSemaphore, semName.c_str());
            SetDebugName(fd.inFlightFence, vk::ObjectType::eFence, fenceName.c_str());
            m_FrameData.push_back(fd);
        }

        CompileShader("static_triangle", "Shaders/static_triangle.slang");

        // Setup the static dynamic state for now
        m_DynamicStates.push_back(vk::DynamicState::eViewport);
        m_DynamicStates.push_back(vk::DynamicState::eScissor);

        m_Log->Info("Vulkan Context created");
    }

    VulkanContext::~VulkanContext()
    {
        //m_Log->Info("Destroying Vulkan Context");
    }

    DeviceHandle VulkanContext::CreateDevice()
    {
        return Quest::MakeRefCounted<VulkanDevice>(this);
    }

    void VulkanContext::BeginFrame()
    {
        auto frameData = GetFrameData();
        auto fenceRes = m_Device.waitForFences(frameData.inFlightFence, true, std::numeric_limits<uint64_t>::max());
        VK_CHECK(fenceRes);
        m_Device.resetFences(frameData.inFlightFence);

        auto [result, imageIndex] = m_Device.acquireNextImageKHR(m_Swapchain, std::numeric_limits<uint64_t>::max(), frameData.presentCompleteSemaphore, nullptr);
        VK_CHECK(result);
        m_SwapchainIndex = imageIndex;
    }
    void VulkanContext::EndFrame()
    {
        //m_Device.waitIdle();
    }

    void VulkanContext::PresentFrame()
    {
        vk::Semaphore renderFinishedSemaphore = GetRenderFinishedSemaphore();
        vk::PresentInfoKHR presentInfoKHR{
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &renderFinishedSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &m_Swapchain,
            .pImageIndices = &m_SwapchainIndex};

        vk::Result result = GetQueue(qrhi::QueueType::Present)->GetQueue().presentKHR(presentInfoKHR);

        m_FrameCount++;
    }

    void VulkanContext::Shutdown()
    {
        m_Log->Info("Shutting down and cleaning up resources of Vulkan Context");

        m_Device.waitIdle();

        for (auto fd : m_FrameData)
        {
            m_Device.destroyFence(fd.inFlightFence, nullptr);
            m_Device.destroySemaphore(fd.presentCompleteSemaphore, nullptr);
        }

        DestroySwapchain();

        // Make sure the semaphores inside the queues are deleted before the device is deleted
        for (auto& queue : m_Queues)
            queue.reset();

        vmaDestroyAllocator(m_Allocator);
        m_Device.destroy();

        m_Instance.destroySurfaceKHR(m_Surface);
        m_Instance.destroyDebugUtilsMessengerEXT(m_DebugMessenger);
        m_Instance.destroy();
    }

    void VulkanContext::SetDebugName(const void* handle, const vk::ObjectType objType, const std::string_view& name) const
    {
        if (!handle)
            return;

        auto info = vk::DebugUtilsObjectNameInfoEXT()
            .setObjectType(objType)
            .setObjectHandle(reinterpret_cast<uint64_t>(handle))
            .setPObjectName(name.data());
        m_Device.setDebugUtilsObjectNameEXT(info);
    }

    Queue* VulkanContext::GetQueue(QueueType type) const
    {
        return m_Queues[static_cast<uint32_t>(type)].get();
    }

    FrameData& VulkanContext::GetFrameData()
    {
        LOG_DEBUG("FrameDataIndex: {}", m_FrameCount % (static_cast<int>(m_Desc.framesInFlight) + 1));
        return m_FrameData[m_FrameCount % (static_cast<int>(m_Desc.framesInFlight) + 1)];
    }

    void VulkanContext::CreateSwapchain()
    {
        logInfo("Creating Swapchain");

        vkb::SwapchainBuilder swapchain_builder{m_PhysicalDevice, m_Device, m_Surface};
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
            VK_CHECK(m_Device.createSemaphore(&semaphoreCreateInfo, nullptr, &m_RenderSemaphores[i]));
            SetDebugName(m_RenderSemaphores[i], vk::ObjectType::eSemaphore, "renderSemaphore: " + std::to_string(i));
        }
    }

    void VulkanContext::DestroySwapchain() const
    {
        logInfo("Destroying Swapchain");

        for (int i = 0; i < m_RenderSemaphores.size(); i++)
        {
            m_Device.destroySemaphore(m_RenderSemaphores[i], nullptr);
        }

        for (int i = 0; i < m_SwapchainImageViews.size(); i++)
        {
            m_Device.destroyImageView(m_SwapchainImageViews[i], nullptr);
        }

        m_Device.destroySwapchainKHR(m_Swapchain, nullptr);
    }

    void VulkanContext::RecreateSwapchain()
    {
        DestroySwapchain();
        CreateSwapchain();
    }

    void VulkanContext::TransitionImage(vk::CommandBuffer cmdBuffer, vk::Image image, vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout) const
    {
        vk::ImageMemoryBarrier2 barrier = {};
        barrier.pNext = nullptr;
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.image = image;
        barrier.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        barrier.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        barrier.subresourceRange = vk::ImageSubresourceRange{
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        };

        vk::DependencyInfo dependencyInfo = {
            .dependencyFlags = {},
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &barrier,
        };

        cmdBuffer.pipelineBarrier2(dependencyInfo);
    }
}
