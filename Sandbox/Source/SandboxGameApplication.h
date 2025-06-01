#pragma once
#include "SandboxAPI.h"
#include "Engine/GameApplication.h"
#include "RHI/ResourceTypes.h"

#include <vector>

#include "Renderer/RenderTypes.h"

class SANDBOX_API SandboxGameApplication : public QE::GameApplication
{
public:
    SandboxGameApplication() = default;
    virtual ~SandboxGameApplication() = default;

    virtual void Init() override;
    virtual void Shutdown() override;
    virtual void Update() override;

private:
    std::vector<QE::Vertex> TriangleVertices;
    std::vector<QE::Vertex> RectangleVertices;
	std::vector<std::uint32_t> TriangleIndices;
	std::vector<std::uint32_t> RectangleIndices;
    QE::BufferHandle m_TriangleVertexBuffer;
    QE::BufferHandle m_TriangleIndexBuffer;
    QE::BufferHandle m_RectangleVertexBuffer;
    QE::BufferHandle m_RectangleIndexBuffer;
    QE::Mesh m_TriangleMesh;
    QE::Mesh m_RectangleMesh;
    int selectedMesh = 0;
};
