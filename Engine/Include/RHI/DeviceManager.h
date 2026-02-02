#pragma once

#include "RHI_Common.h"
#include "Device.h"
#include "Framebuffer.h"
#include "RenderPass.h"

#include <vector>

namespace qrhi
{
    class DeviceManager
    {
    public:
        void AddRenderPassToFront(RenderPass* renderpass);
        void AddRenderPassToBack(RenderPass* renderpass);
        void RemoveRenderPass(RenderPass* renderpass);

        DeviceHandle GetDevice() const { return m_Device; }
        Device* GetDevicePtr() const { return m_Device.get(); }
        virtual GraphicsAPI GetGraphicsAPI() const = 0;

        Framebuffer* GetCurrentFramebuffer();
        Framebuffer* GetFramebuffer(uint32_t index);

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void PresentFrame() = 0;

        virtual void Shutdown() = 0;
        virtual ~DeviceManager() = default;
    protected:
        DeviceHandle m_Device = nullptr;

        std::vector<RenderPass*> m_RenderPasses;

        uint32_t m_FrameIndex = 0;
        std::vector<FramebufferHandle> m_SwapchainFramebuffers;

        DeviceManager() = default;
    };

    std::unique_ptr<DeviceManager> CreateDeviceManager(const DeviceDesc& desc);
}