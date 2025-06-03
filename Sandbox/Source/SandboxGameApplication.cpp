#include "SandboxGameApplication.h"
#include "Core/Log.h"

#include "Engine/Engine.h"

#include "Core/Containers/RawBuffer.h"

#include "imgui.h"

#include "Assets/AssetLoader.h"

#include "Core/StringID.h"

void SandboxGameApplication::Init()
{
    using namespace QE;
    LOG_INFO("Sandbox Game Application Initialized");

    StringID test1 = InternString("Test1");
    StringID test2 = InternString("Test2");
    LOG_DEBUG("SID: {}, String: {}", test1, GetStringFromID(test1).data());
    LOG_DEBUG("SID: {}, String: {}", test2, GetStringFromID(test2).data());


    TriangleVertices = {
        {{0.0f, -0.5f, 0.0f}, 0, {1.0f, 1.0f, 1.0f}, 0, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{0.5f, 0.5f, 0.0f}, 0, {1.0f, 1.0f, 1.0f}, 0, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, 0, {1.0f, 1.0f, 1.0f}, 0, {0.0f, 0.0f, 1.0f, 1.0f}}
    };

    TriangleIndices = {
        0, 1, 2
    };

    /*RectangleVertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
    };

    RectangleIndices = {
        0, 1, 2, 2, 3, 0
    };
    */

    Engine* engine = QE::GetEngine();
    GraphicsDevice* device = engine->GetGraphicsDevicePtr();

    // Triangle
    std::vector<std::uint8_t> verticesbuff;
    verticesbuff.resize(TriangleVertices.size() * sizeof(Vertex));
    memcpy(verticesbuff.data(), TriangleVertices.data(), verticesbuff.size());
    BufferDescription verticesDesc = {
        BufferType::Vertex,
        BufferUsage::Default,
        verticesbuff,
        verticesbuff.size() * sizeof(Vertex),
        verticesbuff.size()
    };
    m_TriangleVertexBuffer = device->CreateBuffer(verticesDesc);

    std::vector<std::uint8_t> indicesbuff(TriangleIndices.size());
    indicesbuff.resize(TriangleIndices.size() * sizeof(std::uint32_t));
    memcpy(indicesbuff.data(), TriangleIndices.data(), indicesbuff.size());
    BufferDescription indicesDesc = {
        BufferType::Index,
        BufferUsage::Default,
        indicesbuff,
        indicesbuff.size() * sizeof(std::uint32_t),
        indicesbuff.size()
    };
    m_TriangleIndexBuffer = device->CreateBuffer(indicesDesc);

    /*
    // Rectangle
    RawBuffer rectangleVertices = RawBuffer(RectangleVertices.data(),RectangleVertices.size(), RectangleVertices.size() * sizeof(Vertex));
    BufferDescription rectangleVertexDesc = {
        BufferType::Vertex,
        BufferUsage::Default,
        &rectangleVertices
    };
    m_RectangleVertexBuffer = device->CreateBuffer(rectangleVertexDesc);

    RawBuffer rectangleIndices = RawBuffer(RectangleIndices.data(),RectangleIndices.size(), RectangleIndices.size() * sizeof(std::uint32_t));
    BufferDescription rectangleIndexDesc = {
        BufferType::Index,
        BufferUsage::Default,
        &rectangleIndices
    };
    m_RectangleIndexBuffer = device->CreateBuffer(rectangleIndexDesc);

    m_RectangleMesh = {
        m_RectangleVertexBuffer,
        m_RectangleIndexBuffer
    }*/;

    m_TriangleMesh = {
        m_TriangleVertexBuffer,
        m_TriangleIndexBuffer
    };

    //auto m = QE::LoadModel("TestMesh/viking_room.obj");
    auto m = QE::LoadModel("TestMesh/basicmesh.glb");
    m_Model = m.value();
}

void SandboxGameApplication::Shutdown()
{
    LOG_INFO("Sandbox Game Application Shutdown");
}

void SandboxGameApplication::Update()
{
    using namespace QE;
    if (QE::GetEngine()->GetInput().IsKeyPressed(QE::Key::A))
    {
        LOG_INFO("A Pressed from Sandbox");
    }

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
    GetEngine()->GetGraphicsDevicePtr()->DrawMesh(m_Model.Meshes[2]);
    //GetEngine()->GetGraphicsDevicePtr()->DrawMesh(m_TriangleMesh);

    // Render ImGui
    // ImGui fps window
    {
        ImGui::Begin("FPS");
        ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
        ImGui::End();
    }
}
