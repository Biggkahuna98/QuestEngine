#pragma once
#include "Core/RefCounting.h"
#include "RHICommon.h"
#include "Device.h"

namespace Quest
{
    struct ContextDesc
    {
        MessageCallback* messageCallback = nullptr;
        bool enableAPISpecificValidation = true;
        Window* window = nullptr;
    };

    class GraphicsContext : public RefCounted
    {
    public:
        virtual Device* CreateDevice(DeviceDesc desc) = 0;
    };

    using GraphicsContextHandle = RefCountPtr<GraphicsContext>;
}

using CreateGraphicsContextFn = Quest::GraphicsContext* (*)(Quest::ContextDesc desc);
using DestroyGraphicsContextFn = void (*)(Quest::GraphicsContext* context);