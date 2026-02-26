#pragma once

#include "Common.h"
#include "Device.h"

namespace qrhi
{
    struct ContextDesc
    {
        API graphicsAPI = API::Vulkan;
    };

    // The graphics context, stores whatever is necessary for the graphics API and manages some higher level
    // constructs, like frames in flight
    // also creates the graphics device
    class Context
    {
    public:
        virtual ~Context() = default;
        virtual DeviceHandle CreateDevice(DeviceDesc) = 0;
    };

    using ContextHandle = std::shared_ptr<Context>;

    ContextHandle CreateContext(ContextDesc desc);
}