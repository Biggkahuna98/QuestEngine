#pragma once

#include "Core/RHI/GraphicsDevice.h"

namespace Quest::Vulkan
{
    class Device : public GraphicsDevice
    {
    public:
        Device(const DeviceDesc& desc);
        ~Device() override;

        void WaitIdle() override;
    private:
        DeviceDesc m_Desc;
    };
}