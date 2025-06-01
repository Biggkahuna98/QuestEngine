#pragma once
#include "Core/Core.h"
#include "Core/Containers/RawBuffer.h"
#include "RHISettings.h"
#include <glm/glm.hpp>

namespace QE
{
    // Vertex - uv_x and uv_y are tex coords
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Color;
    };

    // Handles
    struct BufferHandle
    {
        std::uint32_t Value;

        bool operator==(const BufferHandle& rhs) const
        {
            return rhs.Value == Value;
        }
    };
    struct TextureHandle { std::uint32_t Value; };

    // Descriptions
    struct BufferDescription
    {
        BufferType Type;
        RawBuffer* Data;
    };

    struct TextureDescription
    {

    };
}
