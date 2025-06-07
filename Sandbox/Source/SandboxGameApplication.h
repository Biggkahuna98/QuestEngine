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
    std::vector<QE::Vertex> RectangleVertices;
	std::vector<std::uint32_t> RectangleIndices;
    QE::BufferHandle m_RectangleVertexBuffer;
    QE::BufferHandle m_RectangleIndexBuffer;
    QE::MeshHandle m_RectangleMesh;
    int selectedMesh = 0;
    QE::Model m_Model;
    QE::TextureHandle m_Texture;
};
