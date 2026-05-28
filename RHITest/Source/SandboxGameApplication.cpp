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

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

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
    qrhi::CompileShader("uniform_buffer", "Shaders/uniform_buffer.slang");

    auto GraphicsDevice = engine->GetGraphicsDevice();

    qrhi::CommandListDesc commandListDesc{};
    commandListDesc.type = qrhi::QueueType::Graphics;
    m_GraphicsCommandList = GraphicsDevice->CreateCommandList(commandListDesc);

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

    const std::vector<qrhi::Vertex> vertices3 = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    qrhi::BufferDesc bufferDesc{};
    bufferDesc.type = qrhi::BufferType::Vertex;
    bufferDesc.sizeInBytes = sizeof(vertices3[0]) * vertices3.size();
    bufferDesc.size = vertices3.size();
    m_VertexBuffer = GraphicsDevice->CreateBuffer(bufferDesc);

    void* ptr = GraphicsDevice->MapBuffer(m_VertexBuffer.Get());
    memcpy(ptr, vertices3.data(), sizeof(vertices3[0]) * vertices3.size());
    GraphicsDevice->UnmapBuffer(m_VertexBuffer.Get());

    qrhi::BufferDesc bufferDesc2{};
    bufferDesc2.type = qrhi::BufferType::Index;
    bufferDesc2.size = indices.size();
    bufferDesc2.sizeInBytes = sizeof(indices[0]) * indices.size();
    LOG_INFO("Size: {}, SizeInBytes: {}", bufferDesc2.size, bufferDesc2.sizeInBytes);
    m_IndexBuffer = GraphicsDevice->CreateBuffer(bufferDesc2);

    ptr = GraphicsDevice->MapBuffer(m_IndexBuffer.Get());
    memcpy(ptr, indices.data(), sizeof(indices[0]) * indices.size());
    GraphicsDevice->UnmapBuffer(m_IndexBuffer.Get());

    m_VertexBuffer->SetDebugName("Rectangle Buffer - Vertices");
    m_IndexBuffer->SetDebugName("Rectangle Buffer - Indices");

    qrhi::BindingLayoutDesc bindingLayoutDesc{};
    bindingLayoutDesc.AddItem(
        qrhi::BindingLayoutItem::UniformBuffer(0)
    );
    m_BindingLayout = GraphicsDevice->CreateBindingLayout(bindingLayoutDesc);

    qrhi::BufferDesc uniformBufferDesc{};
    uniformBufferDesc.type = qrhi::BufferType::Uniform;
    uniformBufferDesc.size = 1;
    uniformBufferDesc.sizeInBytes = sizeof(UniformBufferObject);
    m_UniformBuffer = GraphicsDevice->CreateBuffer(uniformBufferDesc);
    m_MappedUniformBuffer = GraphicsDevice->MapBuffer(m_UniformBuffer.Get());

    m_UniformBuffer->SetDebugName("Uniform Buffer #1");

    qrhi::BindingSetDesc bindingSetDesc{};
    bindingSetDesc.AddItem(
        qrhi::BindingSetItem::UniformBuffer(0, m_UniformBuffer.Get())
    );

    m_BindingSet = GraphicsDevice->CreateBindingSet(bindingSetDesc, m_BindingLayout.Get());

    /*qrhi::GraphicsPipelineDesc pipelineDesc{};
    qrhi::ShaderDesc shaderDesc{};
    shaderDesc.type = qrhi::ShaderType::Vertex;
    shaderDesc.name = "vertex_buffer.spv";
    qrhi::ShaderHandle vertexShader = GraphicsDevice->CreateShader(shaderDesc);
    pipelineDesc.vertexShader = vertexShader;
    pipelineDesc.bindingLayouts.push_back(m_BindingLayout);
    m_GraphicsPipeline = GraphicsDevice->CreateGraphicsPipeline(pipelineDesc);*/

    qrhi::GraphicsPipelineDesc pipelineDesc{};
    qrhi::ShaderDesc shaderDesc{};
    shaderDesc.type = qrhi::ShaderType::Vertex;
    shaderDesc.name = "uniform_buffer.spv";
    qrhi::ShaderHandle vertexShader = GraphicsDevice->CreateShader(shaderDesc);
    pipelineDesc.vertexShader = vertexShader;
    pipelineDesc.bindingLayouts.push_back(m_BindingLayout);
    m_GraphicsPipeline = GraphicsDevice->CreateGraphicsPipeline(pipelineDesc);

    m_GraphicsPipeline->SetDebugName("Rectangle Pipeline");

    LOG_INFO("ShaderType: Vertex =  {:8b}", static_cast<uint16_t>(qrhi::ShaderType::Vertex));
    LOG_INFO("ShaderType: Fragment = {:8b}", static_cast<uint16_t>(qrhi::ShaderType::Fragment));
    LOG_INFO("ShaderType: Compute =  {:8b}", static_cast<uint16_t>(qrhi::ShaderType::Compute));
    LOG_INFO("ShaderType: Vertex + Fragment = {:8b}", static_cast<uint16_t>(qrhi::ShaderType::Vertex | qrhi::ShaderType::Fragment));
}

void SandboxGameApplication::Shutdown()
{
    LOG_INFO("Sandbox Game Application Shutdown");

    auto GraphicsDevice = Quest::GetEngine()->GetGraphicsDevice();
    GraphicsDevice->UnmapBuffer(m_UniformBuffer.Get());

    m_BindingSet.Reset();
    m_BindingLayout.Reset();
    m_VertexBuffer.Reset();
    m_IndexBuffer.Reset();
    m_UniformBuffer.Reset();
    m_GraphicsPipeline.Reset();
    m_GraphicsCommandList.Reset();
}

float updateUniformBuffer(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time       = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    return time;
}

void SandboxGameApplication::Update()
{
    PROFILE_SCOPE("SandboxGameApplication::Update");
    using namespace Quest;

    m_GraphicsCommandList->Open();
    qrhi::GraphicsState state{};
    state.vertexBuffer = m_VertexBuffer;
    state.indexBuffer = m_IndexBuffer;
    state.pipeline = m_GraphicsPipeline.Get();
    state.bindingSet = m_BindingSet;
    m_GraphicsCommandList->SetGraphicsState(state);

    UniformBufferObject ubo{};
    float time = updateUniformBuffer(1);
    //ubo.model = rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.model = glm::mat4(0.25f);
    ubo.view = lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 10.0f);

    //ubo.projection[1][1] *= -1;
    memcpy(m_MappedUniformBuffer, &ubo, sizeof(ubo));

    qrhi::DrawArguments drawArguments{};
    drawArguments.vertexCount = 4;
    drawArguments.instanceCount = 1;
    m_GraphicsCommandList->DrawIndexed(drawArguments);

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

    ShutdownEngineEntrypoint();
    delete app;

    return 0;
}