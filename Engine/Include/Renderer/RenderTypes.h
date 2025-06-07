#pragma once
#include "RHI/ResourceTypes.h"
#include <glm/glm.hpp>
#include <string>

namespace QE
{
    // Higher level types
    struct QUEST_API Model
    {
        std::vector<MeshHandle> Meshes;
        std::string Name = "Unnamed Model";
    };

    struct QUEST_API ModelViewProjection
    {
        glm::mat4 Model;
        glm::mat4 View;
        glm::mat4 Projection;
    };
}