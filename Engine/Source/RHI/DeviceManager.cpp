#include "RHI/DeviceManager.h"

#include "VulkanRHI/Vulkan_DeviceManager.h"

namespace qrhi
{
    std::unique_ptr<DeviceManager> CreateDeviceManager(const DeviceDesc& desc)
    {
        if (desc.api == GraphicsAPI::Vulkan)
            return std::make_unique<vulkan::VulkanDeviceManager>(desc);
        return nullptr;
    }

    void DeviceManager::AddRenderPassToFront(RenderPass* renderpass)
    {
        m_RenderPasses.insert(m_RenderPasses.begin(), renderpass);
    }

    void DeviceManager::AddRenderPassToBack(RenderPass* renderpass)
    {
        m_RenderPasses.push_back(renderpass);
    }

    void DeviceManager::RemoveRenderPass(RenderPass* renderpass)
    {
        for (std::vector<RenderPass*>::iterator iter = m_RenderPasses.begin(); iter != m_RenderPasses.end(); ++iter)
        {
            if (*iter == renderpass)
            {
                m_RenderPasses.erase(iter);
                break;
            }
        }
    }

    Framebuffer* DeviceManager::GetCurrentFramebuffer()
    {
        return nullptr;
    }

    Framebuffer* DeviceManager::GetFramebuffer(uint32_t index)
    {
        return nullptr;
    }
}
