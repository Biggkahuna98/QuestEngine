#pragma once
#include "Core/Window.h"

namespace Quest
{
    class CORE_API NullWindow : public Window
    {
    public:
        NullWindow() {}
        ~NullWindow() noexcept override {};
        void* GetNativeWindow() override { return nullptr; };
        bool ProcessEvents() override { return false; };
        void PauseWindow() override {};
        void SetMouseInputProcessing(bool processing) override {};
    };
}
