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

    qrhi::ShaderDesc sdesc{};
    sdesc.name = "static_triangle.spv";
    sdesc.type = qrhi::ShaderType::CombinedTypes;
    sdesc.shaderTypeMask = static_cast<uint32_t>(qrhi::ShaderType::Vertex | qrhi::ShaderType::Fragment);
    qrhi::ShaderHandle shader = engine->GetGraphicsDevice()->CreateShader(sdesc);
}

void SandboxGameApplication::Shutdown()
{
    LOG_INFO("Sandbox Game Application Shutdown");
}

void SandboxGameApplication::Update()
{
    PROFILE_SCOPE("SandboxGameApplication::Update");
    using namespace Quest;
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