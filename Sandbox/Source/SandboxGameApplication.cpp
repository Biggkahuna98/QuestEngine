#include "SandboxGameApplication.h"
#include "Core/Log.h"

#include "Engine/Engine.h"

#include "Core/Containers/RawBuffer.h"

#include "imgui.h"

#include "Assets/AssetLoader.h"

#include "Core/StringID.h"
#include "Core/Events/EventManager.h"
#include "Core/Events/EngineEvents.h"

void SandboxGameApplication::Init()
{
    using namespace QE;
    LOG_INFO("Sandbox Game Application Initialized");

    StringID test1 = InternString("Test1");
    StringID test2 = InternString("Test2");
    LOG_DEBUG("SID: {}, String: {}", test1, GetStringFromID(test1).data());
    LOG_DEBUG("SID: {}, String: {}", test2, GetStringFromID(test2).data());

    RectangleVertices = {
        {{-0.5f, -0.5f, 0.0f}, 0, {1.0f, 1.0f, 1.0f}, 0, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f, 0.0f}, 0, {1.0f, 1.0f, 1.0f}, 0, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{0.5f, 0.5f, 0.0f}, 0, {1.0f, 1.0f, 1.0f}, 0, {0.0f, 0.0f, 1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, 0, {1.0f, 1.0f, 1.0f}, 0, {1.0f, 1.0f, 1.0f, 1.0f}}
    };

    RectangleIndices = {
        0, 1, 2, 2, 3, 0
    };

    Engine* engine = QE::GetEngine();
    GraphicsDevice* device = engine->GetGraphicsDevicePtr();

    m_RectangleMesh = device->CreateMesh(RectangleVertices, RectangleIndices);

    //auto m = QE::LoadModel("Models/viking_room.obj");
    auto m = QE::LoadModel("Models/basicmesh.glb");
    m_Model = m.value();
    LOG_DEBUG("Model mesh count: {}", m_Model.Meshes.size());

    auto tex = QE::LoadTexture("Textures/texture.jpg");
    m_Texture = tex.value();
}

void SandboxGameApplication::Shutdown()
{
    LOG_INFO("Sandbox Game Application Shutdown");
}

void SandboxGameApplication::Update()
{
    using namespace QE;

    //ImGui::ShowDemoWindow();
    // ImGui to decide which buffer to draw
    {
        if (ImGui::Begin("Draw Mesh"))
        {
            ImGui::SliderInt("Buffer Index", &selectedMesh, 0, 1);
        }
        ImGui::End();
    }

    //Mesh meshToDraw = selectedMesh == 0 ? m_TriangleMesh : m_RectangleMesh;

    // Draw the triangle
    //GetEngine()->GetGraphicsDevicePtr()->DrawMesh(m_Model.Meshes[2]);
    GetEngine()->GetGraphicsDevicePtr()->DrawMesh(m_RectangleMesh);

    // Render ImGui
    // ImGui fps window
    {
        ImGui::Begin("FPS");
        ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
        ImGui::End();
    }
}
