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
}