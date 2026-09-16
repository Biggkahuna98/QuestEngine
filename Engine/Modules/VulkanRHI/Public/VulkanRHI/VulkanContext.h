#pragma once

#include <VulkanRHI/VMA_Usage.h>
#include "VulkanRHI/VulkanRHIExport.h"
#include "Core/RHI/GraphicsContext.h"

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_enum_string_helper.h>

#include <format>

namespace Quest::Vulkan
{
    extern MessageCallback* g_Logger;

    class VULKANRHI_API Context : public GraphicsContext
    {
    public:
        Context(const ContextDesc& desc);
        ~Context() override;

        GraphicsDevice* CreateDevice(const DeviceDesc& desc) override;
    private:
        ContextDesc m_Desc;

        MessageCallback* m_Log = nullptr;

        // Vulkan objects
        vk::Instance m_Instance;
        vk::DebugUtilsMessengerEXT m_DebugMessenger;
        vk::Extent2D m_WindowExtent;
        vk::SurfaceKHR m_Surface;

        vk::PhysicalDevice m_PhysicalDevice;
        vk::Device m_Device;
        VmaAllocator m_Allocator;

        vk::Queue m_GraphicsQueue;
        vk::Queue m_PresentQueue;
        vk::Queue m_TransferQueue;
        vk::Queue m_ComputeQueue;
    };
}

#define VK_CHECK_RES(res) assert((res) == vk::Result::eSuccess);

#define VK_CHECK(x)                                                                      \
do {                                                                                     \
    vk::Result err = x;                                                                  \
    if (static_cast<uint32_t>(err)) {                                                    \
        g_Logger->Error(std::format("Vulkan", "Detected Vulkan error: {}", vk::to_string(err)));        \
        abort();                                                                         \
    }                                                                                    \
} while (0)

// Old C one for any needs, like VMA
#define VK_CHECK_OLD(x)                                                                  \
do {                                                                                     \
    VkResult err = x;                                                                    \
    if (err) {                                                                           \
        g_Logger->Error(std::format("Vulkan", "Detected Vulkan error: {}", string_VkResult(err)));      \
        abort();                                                                         \
    }                                                                                    \
} while (0)

extern "C" VULKANRHI_API Quest::GraphicsContext* CreateGraphicsContext(Quest::ContextDesc desc);
extern "C" VULKANRHI_API void DestroyGraphicsContext(Quest::GraphicsContext* context);