#include "SandboxGameApplication.h"
#include "Core/Log.h"

#include "Engine/Engine.h"

#include "Core/Containers/RawBuffer.h"

#include "imgui.h"

void SandboxGameApplication::Init()
{
    LOG_INFO("Sandbox Game Application Initialized");

    TriangleVertices = {
        {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    };

    TriangleIndices = {
        0, 1, 2
    };

    RectangleVertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
    };

    RectangleIndices = {
        0, 1, 2, 2, 3, 0
    };

    using namespace QE;
    Engine* engine = QE::GetEngine();
    GraphicsDevice* device = engine->GetGraphicsDevicePtr();

    // Triangle
    RawBuffer triangleVertices = RawBuffer(TriangleVertices.data(),TriangleVertices.size(), TriangleVertices.size() * sizeof(Vertex));
    BufferDescription triangleVertexDesc = {
        BufferType::Vertex,
        BufferUsage::Default,
        &triangleVertices
    };
    m_TriangleVertexBuffer = device->CreateBuffer(triangleVertexDesc);

    RawBuffer triangleIndices = RawBuffer(TriangleIndices.data(),TriangleIndices.size(), TriangleIndices.size() * sizeof(std::uint32_t));
    BufferDescription triangleIndexDesc = {
        BufferType::Index,
        BufferUsage::Default,
        &triangleIndices
    };
    m_TriangleIndexBuffer = device->CreateBuffer(triangleIndexDesc);

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

    m_TriangleMesh = {
        m_TriangleVertexBuffer,
        m_TriangleIndexBuffer
    };

    m_RectangleMesh = {
        m_RectangleVertexBuffer,
        m_RectangleIndexBuffer
    };
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

    Mesh meshToDraw = selectedMesh == 0 ? m_TriangleMesh : m_RectangleMesh;

    // Draw the triangle
    GetEngine()->GetGraphicsDevicePtr()->DrawMesh(meshToDraw);

    // Render ImGui
    // ImGui fps window
    {
        ImGui::Begin("FPS");
        ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
        ImGui::End();
    }
}
