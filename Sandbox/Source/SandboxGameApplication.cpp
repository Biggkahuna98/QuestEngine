#include "SandboxGameApplication.h"
#include "Core/Log.h"

#include "Engine/Engine.h"

void SandboxGameApplication::Init()
{
    LOG_INFO("Sandbox Game Application Initialized");
}

void SandboxGameApplication::Shutdown()
{
    LOG_INFO("Sandbox Game Application Shutdown");
}

void SandboxGameApplication::Update()
{
    //LOG_INFO("Sandbox Game Application Update");
    if (QE::GetEngine()->GetInput().IsKeyPressed(QE::Key::A))
    {
        LOG_INFO("A Pressed from Sandbox");
    }
}
