#include "SandboxGameApplication.h"
#include "Core/Log.h"

#include "Core/Engine.h"

#include "imgui.h"

#include "Assets/AssetLoader.h"

#include "Core/StringID.h"
#include "Core/Events/EventManager.h"
#include "Core/Profiling.h"

void SandboxGameApplication::Init()
{
    using namespace QE;
    LOG_INFO("Sandbox Game Application Initialized");

    StringID test1 = InternString("Test1");
    StringID test2 = InternString("Test2");
    LOG_DEBUG("SID: {}, String: {}", test1, GetStringFromID(test1).data());
    LOG_DEBUG("SID: {}, String: {}", test2, GetStringFromID(test2).data());

    glm::vec3 recNormals = {0.0f, 0.0f, 1.0f};
    RectangleVertices = {
        {{-0.5f, -0.5f, 0.0f}, 0, recNormals, 0, {1.0f, 1.0f, 1.0f, 1.0f}},
        {{0.5f, -0.5f, 0.0f}, 1.0f, recNormals, 0, {1.0f, 1.0f, 1.0f, 1.0f}},
        {{0.5f, 0.5f, 0.0f}, 1.0f, recNormals, 1.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, 0, recNormals, 1.0f, {1.0f, 1.0f, 1.0f, 1.0f}}
    };

    RectangleIndices = {
        0, 1, 2, 2, 3, 0
    };

    Engine* engine = QE::GetEngine();
    GraphicsDevice* device = engine->GetGraphicsDevicePtr();

    // Create pipelines needed
    CreatePipelines();

    m_RectangleMesh = device->CreateMesh(RectangleVertices, RectangleIndices);

    //auto m = QE::LoadModel("Models/viking_room.obj", true, true);
    //auto m = QE::LoadModel("Models/basicmesh.glb");
    //auto m = QE::LoadModel("Models/NewSponza_Main_glTF_003.gltf");
    auto m = LoadModel("Models/monkey.glb");
    m_Model = m.value();
    LOG_DEBUG("Model mesh count: {}", m_Model.Meshes.size());

    //auto tex = QE::LoadTexture("Textures/viking_room.png");
    auto tex = QE::LoadTexture("Textures/texture.jpg");
    m_Texture = tex.value();
}

void SandboxGameApplication::Shutdown()
{
    LOG_INFO("Sandbox Game Application Shutdown");
}

void SandboxGameApplication::Update()
{
    PROFILE_SCOPE("SandboxGameApplication::Update");
    using namespace QE;

    //Mesh meshToDraw = selectedMesh == 0 ? m_TriangleMesh : m_RectangleMesh;

    // Draw the triangle
    //GetEngine()->GetGraphicsDevicePtr()->DrawMesh(m_Model.Meshes[2], &m_Texture);
    //GetEngine()->GetGraphicsDevicePtr()->DrawMesh(m_RectangleMesh, &m_Texture);
    //GetEngine()->GetGraphicsDevice().DrawMesh(m_Model.Meshes[0], &m_Texture);
    auto device = GetEngine()->GetGraphicsDevicePtr();

    PipelineHandle pipeline = selectedPipeline == 0 ? m_StandardPipeline : m_WireframePipeline;

    device->BeginRenderPass({pipeline});

    for (const auto& mesh : m_Model.Meshes)
        device->DrawMesh(mesh);
    //device->DrawMesh(m_Model.Meshes[2]);
    //GetEngine()->GetGraphicsDevicePtr()->DrawMesh(m_Model.Meshes[0]);

    // Render ImGui
    // ImGui fps window
    {
        ImGui::Begin("FPS");
        ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
        ImGui::End();
    }

    {
        ImGui::Begin("Pipeline");
        ImGui::SliderInt("Pipeline", &selectedPipeline, 0, 1);
        ImGui::End();
    }

    device->EndRenderPass({pipeline});
}

void SandboxGameApplication::CreatePipelines()
{
    using namespace QE;

    auto engine = QE::GetEngine();
    auto device = engine->GetGraphicsDevicePtr();

    // Standard pipeline
    PipelineDescription standardPipelineDesc{};
    standardPipelineDesc.Shaders = {
        { "colored_triangle_mesh-vert.spv", 0xFFFFFFFF, ShaderStage::Vertex },
        { "colored_triangle-frag.spv", 0xFFFFFFFF, ShaderStage::Fragment }
    };
    standardPipelineDesc.Topology = PipelineTopology::TriangleList;
    standardPipelineDesc.PrimitiveRestart = false;
    standardPipelineDesc.PolygonMode = PolygonMode::Fill;
    standardPipelineDesc.CullMode = CullMode::Back;
    standardPipelineDesc.WindingOrder = WindingOrder::CounterClockwise;
    standardPipelineDesc.DepthTest = true;
    standardPipelineDesc.DepthWrite = true;
    standardPipelineDesc.DepthCompareOp = DepthCompareOp::GreaterEqual;
    standardPipelineDesc.Blending = true;
    standardPipelineDesc.BlendingType = BlendingType::Alpha;

    m_StandardPipeline = device->CreatePipeline(standardPipelineDesc);

    // Wireframe pipeline
    PipelineDescription wireframePipelineDesc{};
    wireframePipelineDesc.Shaders = {
        { "colored_triangle_mesh-vert.spv", 0xFFFFFFFF, ShaderStage::Vertex },
        { "colored_triangle-frag.spv", 0xFFFFFFFF, ShaderStage::Fragment }
    };
    wireframePipelineDesc.Topology = PipelineTopology::TriangleList;
    wireframePipelineDesc.PrimitiveRestart = false;
    wireframePipelineDesc.PolygonMode = PolygonMode::Line;
    wireframePipelineDesc.CullMode = CullMode::Back;
    wireframePipelineDesc.WindingOrder = WindingOrder::CounterClockwise;
    wireframePipelineDesc.DepthTest = true;
    wireframePipelineDesc.DepthWrite = true;
    wireframePipelineDesc.DepthCompareOp = DepthCompareOp::GreaterEqual;
    wireframePipelineDesc.Blending = true;
    wireframePipelineDesc.BlendingType = BlendingType::Alpha;

    m_WireframePipeline = device->CreatePipeline(wireframePipelineDesc);
}
