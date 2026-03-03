#pragma once

#include "QRHI.h"
#include "Device.h"
#include "Core/Window.h"
#include "Utility/RefCounting.h"

namespace Quest
{
    class Window;
}

namespace qrhi
{
    enum class FramesInFlight
    {
        One,
        Two,
        Three,

        Count
    };

    struct ContextDesc
    {
        API graphicsAPI = API::Vulkan;
        Quest::Window* window = nullptr;
        MessageCallback* messageCallback = nullptr;
        bool enableAPISpecificValidation = true;
        FramesInFlight framesInFlight = FramesInFlight::One; // Only a request, may be ignored
    };

    // The graphics context, stores whatever is necessary for the graphics API and manages some higher level
    // constructs, like frames in flight
    class Context : public Quest::RefCounted
    {
    public:
        virtual ~Context() = default;

        // Interface to create GPU resources
        virtual DeviceHandle CreateDevice() = 0;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void PresentFrame() = 0;

        virtual void Shutdown() = 0;
    };

    using ContextHandle = Quest::RefCountPtr<Context>;

    ContextHandle CreateContext(ContextDesc desc);
}