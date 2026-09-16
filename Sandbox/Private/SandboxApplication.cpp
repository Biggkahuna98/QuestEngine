#include "Sandbox/SandboxApplication.h"

#include <Runtime/Engine.h>
#include <Core/Module/ModuleBoilerplate.h>
#include <Core/Window.h>
#include <Core/Log/Log.h>

#include "Core/Log/BasicSink.h"

OVERRIDE_NEW_DELETE

QE_DEFINE_LOG_CATEGORY(SandboxApp);

namespace Sandbox
{
    SandboxApplication::SandboxApplication() = default;

    SandboxApplication::~SandboxApplication() = default;

    void SandboxApplication::Startup()
    {
        m_Window.reset(Quest::CreateWindow("Sandbox", 1920, 1080));
        m_Engine->SetWindow(m_Window.get());

        QE_LOG_ADD_SINK_TO_CATEGORY(SandboxApp, Quest::BasicSink);
        QE_LOG(SandboxApp, Info, "Hello World!");
    }

    void SandboxApplication::Update()
    {
    }

    void SandboxApplication::Shutdown()
    {
    }
}

Quest::Application* CreateApplication()
{
    return new Sandbox::SandboxApplication();
}

void DestroyApplication(Quest::Application* app)
{
    delete app;
}
