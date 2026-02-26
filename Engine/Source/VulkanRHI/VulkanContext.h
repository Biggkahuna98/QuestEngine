#pragma once

#include "RHI/Context.h"

namespace qrhi::vulkan
{
    class VulkanContext : public Context
    {
    public:
        VulkanContext(ContextDesc desc);
        virtual ~VulkanContext() override;

        virtual DeviceHandle CreateDevice(DeviceDesc) override;

    private:
        ContextDesc m_Desc;
    };
}