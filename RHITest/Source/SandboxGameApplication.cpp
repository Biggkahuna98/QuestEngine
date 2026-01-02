#include "SandboxGameApplication.h"
#include "Core/Log.h"

#include "Core/Engine.h"

#include "imgui.h"

#include "Assets/AssetLoader.h"

#include "Core/StringID.h"
#include "Core/Events/EventManager.h"
#include "Core/Profiling.h"

#include "Utility/RefCounting.h"

void RefTest(Quest::RefCountPtr<TestInterface> ref)
{
    LOG_DEBUG("RefCount: {}", ref->GetRefCount());
}

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
