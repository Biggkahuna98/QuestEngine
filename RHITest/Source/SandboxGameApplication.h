#pragma once
#include "SandboxAPI.h"
#include "Engine/GameApplication.h"
#include "RHI/QRHI.h"

#include <vector>

#include "Utility/RefCounting.h"

class SANDBOX_API SandboxGameApplication : public Quest::GameApplication
{
public:
    SandboxGameApplication() = default;
    virtual ~SandboxGameApplication() = default;

    virtual void Init() override;
    virtual void Shutdown() override;
    virtual void Update() override;

private:
    qrhi::GraphicsPipelineHandle m_GraphicsPipeline;
    qrhi::CommandListHandle m_GraphicsCommandList;
    qrhi::BufferHandle m_VertexBuffer;

    void CreatePipelines();
};

/*class SANDBOX_API TestInterface : public Quest::IRefCountable
{
public:
    virtual void Interface() = 0;
};

class SANDBOX_API TestStruct : public Quest::RefCounted<TestInterface>
{
public:
    int x = 0;
    TestStruct() = default;
    ~TestStruct() = default;

    void Interface() override { x++; }
};*/