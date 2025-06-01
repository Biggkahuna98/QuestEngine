#pragma once
#include "RHI/ResourceTypes.h"

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
}