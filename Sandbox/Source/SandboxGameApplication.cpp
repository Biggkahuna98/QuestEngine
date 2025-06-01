#include "SandboxGameApplication.h"
#include "Core/Log.h"

#include "Engine/Engine.h"

#include "Core/Containers/RawBuffer.h"

#include "imgui.h"

void SandboxGameApplication::Init()
{
    LOG_INFO("Sandbox Game Application Initialized");

    m_TriangleVertices = {
        {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    };

    m_RectangleVertices = {
        // Triangle 1
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // Bottom-left, Red
        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Bottom-right, Green
        {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // Top-right, Blue

        // Triangle 2
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // Bottom-left, Red
        {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // Top-right, Blue
        {{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}}, // Top-left, Yellow
    };

    using namespace QE;
    Engine* engine = QE::GetEngine();
    GraphicsDevice* device = engine->GetGraphicsDevicePtr();

    RawBuffer triangleVertices = RawBuffer(m_TriangleVertices.data(), m_TriangleVertices.size() * sizeof(Vertex));
    BufferDescription triangleDesc = { BufferType::Vertex, &triangleVertices };
    m_TriangleVertexBuffer = device->CreateBuffer(triangleDesc);

    RawBuffer rectangleVertices = RawBuffer(m_RectangleVertices.data(), m_RectangleVertices.size() * sizeof(Vertex));
    BufferDescription rectangleDesc = { BufferType::Vertex, &rectangleVertices };
    m_RectangleVertexBuffer = device->CreateBuffer(rectangleDesc);
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

    //ImGui::ShowDemoWindow();
    BufferHandle bufferToDraw = m_RectangleVertexBuffer;
    // ImGui to decide which buffer to draw


    // Draw the triangle
    GetEngine()->GetGraphicsDevicePtr()->DrawBuffer(bufferToDraw);

    // Render ImGui
    // ImGui fps window
    {
        ImGui::Begin("FPS");
        ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
        ImGui::End();
    }
}
