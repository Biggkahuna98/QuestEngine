#pragma once
#include "SandboxAPI.h"
#include "Engine/GameApplication.h"
#include "RHI/ResourceTypes.h"

#include <vector>

class SANDBOX_API SandboxGameApplication : public QE::GameApplication
{
public:
    SandboxGameApplication() = default;
    virtual ~SandboxGameApplication() = default;

    virtual void Init() override;
    virtual void Shutdown() override;
    virtual void Update() override;

private:
    std::vector<QE::Vertex> m_TriangleVertices;
    std::vector<QE::Vertex> m_RectangleVertices;
    QE::BufferHandle m_TriangleVertexBuffer;
    QE::BufferHandle m_RectangleVertexBuffer;
};