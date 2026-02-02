#pragma once

#include "Framebuffer.h"

namespace qrhi
{
    class DeviceManager;

    class RenderPass
    {
    public:
        RenderPass(DeviceManager* deviceManager)
            : m_DeviceManager(deviceManager) {}
        virtual ~RenderPass() = default;

        virtual void Render(Framebuffer* framebuffer);

        DeviceManager* GetDeviceManager() const { return m_DeviceManager; }
    private:
        DeviceManager* m_DeviceManager = nullptr;
    };
}