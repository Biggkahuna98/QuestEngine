#include "VulkanRHI/VulkanContext.h"
#include "VulkanRHI/VulkanGraphicsDevice.h"

#include "Core/Module/ModuleBoilerplate.h"

#include "VulkanRHI/VkBootstrap.h"
#include "Core/Window.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

// Vulkan hpp global dispatcher
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

// Module boilerplate
OVERRIDE_NEW_DELETE

namespace Quest::Vulkan
{
    // Workaround for not able to lambda capture for the debug callback
    MessageCallback* g_Logger = nullptr;

    Context::Context(const ContextDesc& desc)
        : m_Desc(desc)
    {
        m_Log = desc.messageCallback;
        g_Logger = desc.messageCallback;

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
                auto formatted = std::format("[{}: {}] {}", vkb::to_string_message_severity(messageSeverity), vkb::to_string_message_type(messageType), pCallbackData->pMessage);
                g_Logger->Error(formatted);
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
        VkSurfaceKHR tmpSurface;
        SDL_Vulkan_CreateSurface(static_cast<SDL_Window*>(desc.window->GetNativeWindow()), m_Instance, nullptr, &tmpSurface);
        m_WindowExtent= {
            static_cast<uint32_t>(desc.window->GetWidth()),
            static_cast<uint32_t>(desc.window->GetHeight())
        };
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

        m_GraphicsQueue = device.get_queue(vkb::QueueType::graphics).value();
        m_PresentQueue = device.get_queue(vkb::QueueType::present).value();
        m_TransferQueue = device.get_queue(vkb::QueueType::transfer).value();
        m_ComputeQueue = device.get_queue(vkb::QueueType::compute).value();

        QE_ASSERT(m_GraphicsQueue);
        QE_ASSERT(m_PresentQueue);
        QE_ASSERT(m_TransferQueue);
        QE_ASSERT(m_ComputeQueue);
    }

    Context::~Context()
    {

    }

    GraphicsDevice* Context::CreateDevice(const DeviceDesc& desc)
    {
        return new Device(desc);
    }
}

Quest::GraphicsContext* CreateGraphicsContext(Quest::ContextDesc desc)
{
    Quest::GraphicsContext* ctx = new Quest::Vulkan::Context(desc);
    return ctx;
}

void DestroyGraphicsContext(Quest::GraphicsContext* context)
{
    delete context;
}
