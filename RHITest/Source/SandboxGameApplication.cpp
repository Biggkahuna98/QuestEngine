#include "SandboxGameApplication.h"

#include "Launch/Entrypoint.h"
#include "Core/Log.h"
#include "Core/Engine.h"

#include "imgui.h"

#include "Assets/AssetLoader.h"

#include "Core/StringID.h"
#include "Core/Events/EventManager.h"
#include "Core/Profiling.h"

#include "Utility/RefCounting.h"
#include "Utility/StaticVector.h"

#include "RHI/QRHI.h"
#include "RHI/ShaderUtils.h"

/*void RefTest(Quest::RefCountPtr<TestInterface> ref)
{
    LOG_DEBUG("RefCount: {}", ref->GetRefCount());
}*/

void SandboxGameApplication::Init()
{
    using namespace Quest;
    LOG_INFO("Sandbox Game Application Initialized");

    StringID test1 = InternString("Test1");
    StringID test2 = InternString("Test2");
    LOG_DEBUG("SID: {}, String: {}", test1, GetStringFromID(test1).data());
    LOG_DEBUG("SID: {}, String: {}", test2, GetStringFromID(test2).data());

    glm::vec3 recNormals = {0.0f, 0.0f, 1.0f};
    /*RectangleVertices = {
        {{-0.5f, -0.5f, 0.0f}, 0, recNormals, 0, {1.0f, 1.0f, 1.0f, 1.0f}},
        {{0.5f, -0.5f, 0.0f}, 1.0f, recNormals, 0, {1.0f, 1.0f, 1.0f, 1.0f}},
        {{0.5f, 0.5f, 0.0f}, 1.0f, recNormals, 1.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, 0, recNormals, 1.0f, {1.0f, 1.0f, 1.0f, 1.0f}}
    };

    RectangleIndices = {
        0, 1, 2, 2, 3, 0
    };*/

    Engine* engine = Quest::GetEngine();
    Quest::StaticVector<int, 10> vec{};
    LOG_INFO("Size: {}", vec.size());
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    LOG_INFO("Size: {}", vec.size());
    LOG_INFO("Value: {}", vec[2]);

    qrhi::CompileShader("static_triangle", "Shaders/static_triangle.slang");
    qrhi::CompileShader("vertex_buffer", "Shaders/vertex_buffer.slang");

    auto GraphicsDevice = engine->GetGraphicsDevice();

    qrhi::CommandListDesc commandListDesc{};
    commandListDesc.type = qrhi::QueueType::Graphics;
    m_GraphicsCommandList = GraphicsDevice->CreateCommandList(commandListDesc);

    qrhi::GraphicsPipelineDesc pipelineDesc{};
    qrhi::ShaderDesc shaderDesc{};
    shaderDesc.type = qrhi::ShaderType::Vertex;
    shaderDesc.name = "vertex_buffer.spv";
    qrhi::ShaderHandle vertexShader = GraphicsDevice->CreateShader(shaderDesc);
    pipelineDesc.vertexShader = vertexShader;
    m_GraphicsPipeline = GraphicsDevice->CreateGraphicsPipeline(pipelineDesc);

    const std::vector<qrhi::Vertex> vertices1 = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };

    const std::vector<qrhi::Vertex> vertices = {
        {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };

    qrhi::BufferDesc bufferDesc{};
    bufferDesc.type = qrhi::BufferType::Vertex;
    bufferDesc.sizeInBytes = sizeof(vertices[0]) * vertices.size();
    m_VertexBuffer = GraphicsDevice->CreateBuffer(bufferDesc);

    void* ptr = GraphicsDevice->MapBuffer(m_VertexBuffer.Get());
    memcpy(ptr, vertices.data(), sizeof(vertices[0]) * vertices.size());
    GraphicsDevice->UnmapBuffer(m_VertexBuffer.Get());
}

void SandboxGameApplication::Shutdown()
{
    LOG_INFO("Sandbox Game Application Shutdown");

    m_VertexBuffer.Reset();
    m_GraphicsPipeline.Reset();
    m_GraphicsCommandList.Reset();
}

void SandboxGameApplication::Update()
{
    PROFILE_SCOPE("SandboxGameApplication::Update");
    using namespace Quest;

    m_GraphicsCommandList->Open();
    qrhi::GraphicsState state{};
    state.vertexBuffer = m_VertexBuffer;
    state.pipeline = m_GraphicsPipeline.Get();
    m_GraphicsCommandList->SetGraphicsState(state);

    qrhi::DrawArguments drawArguments{};
    drawArguments.vertexCount = 3;
    drawArguments.instanceCount = 1;
    m_GraphicsCommandList->Draw(drawArguments);

    m_GraphicsCommandList->Close();
}

void SandboxGameApplication::CreatePipelines()
{
    using namespace Quest;

    auto engine = Quest::GetEngine();

}

int main(int argc, char** argv)
{
    InitializeEngineEntrypoint();

    Quest::GameApplication* app = new SandboxGameApplication();
    Quest::GetEngine()->SetGameApplication(app);

    RunEngine();

    delete app;
    ShutdownEngineEntrypoint();

    return 0;
}