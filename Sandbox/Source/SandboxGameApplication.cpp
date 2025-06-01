#include "SandboxGameApplication.h"
#include "Core/Log.h"

#include "Engine/Engine.h"

#include "imgui.h"

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
    using namespace QE;
    //LOG_INFO("Sandbox Game Application Update");
    if (QE::GetEngine()->GetInput().IsKeyPressed(QE::Key::A))
    {
        LOG_INFO("A Pressed from Sandbox");
    }

    // Render ImGui
    // ImGui fps window
    ImGui::Begin("FPS");
    ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
    ImGui::End();
}
