#pragma once
#include "RHI/ResourceTypes.h"
#include <glm/glm.hpp>

namespace QE
{
    // Higher level types
    struct QUEST_API Mesh
    {
        BufferHandle VertexBuffer;
        BufferHandle IndexBuffer;
    };

    struct QUEST_API Model
    {
        std::vector<Mesh> Meshes;
    };

    struct QUEST_API ModelViewProjection
    {
        glm::mat4 Model;
        glm::mat4 View;
        glm::mat4 Projection;
    };
}