#pragma once
#include "Core/Window.h"
#include "Core/Core.h"

namespace Quest
{
    struct DeviceDesc
    {
        Window* window = nullptr;
    };

    class GraphicsDevice
    {
    public:
        virtual ~GraphicsDevice() = default;
        virtual void WaitIdle() = 0;
    };
}