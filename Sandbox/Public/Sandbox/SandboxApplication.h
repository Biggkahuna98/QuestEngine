#pragma once
#include "Sandbox/SandboxExport.h"

#include <Runtime/Application.h>
#include <Core/Window.h>
#include <Core/Log/Log.h>

#include <memory>

QE_DECLARE_LOG_CATEGORY(SandboxApp, Info);

namespace Sandbox
{
    class SANDBOX_API SandboxApplication : public Quest::Application
    {
    public:
        SandboxApplication();
        ~SandboxApplication() override;

        void Startup() override;
        void Update() override;
        void Shutdown() override;
    private:
        std::unique_ptr<Quest::Window> m_Window;
    };
}

extern "C" SANDBOX_API Quest::Application* CreateApplication();
extern "C" SANDBOX_API void DestroyApplication(Quest::Application* app);