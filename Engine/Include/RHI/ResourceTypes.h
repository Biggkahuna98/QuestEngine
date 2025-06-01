#pragma once
#include "Core/Core.h"
#include "Core/Containers/RawBuffer.h"
#include "RHISettings.h"
#include <glm/glm.hpp>

namespace QE
{
    // Vertex - uv_x and uv_y are tex coords
    struct QUEST_API Vertex
    {
        glm::vec3 Position;
        glm::vec3 Color;
    };

    // Handles
    struct QUEST_API BufferHandle
    {
        std::uint32_t Value;

        bool operator==(const BufferHandle& other) const
        {
            return other.Value == Value;
        }
    };

    struct QUEST_API TextureHandle
    {
        std::uint32_t Value;

        bool operator==(const TextureHandle& other) const
        {
            return other.Value == Value;
        }
    };

    // Descriptions
    struct QUEST_API BufferDescription
    {
        BufferType Type;
        BufferUsage Usage = BufferUsage::Default;
        RawBuffer* Data;
    };

    struct QUEST_API TextureDescription
    {

    };
}

// Hash functions for handles
template<>
struct std::hash<QE::BufferHandle>
{
    std::size_t operator()(const QE::BufferHandle& handle) const noexcept
    {
        return std::hash<std::uint32_t>()(handle.Value);
    }
};

template<>
struct std::hash<QE::TextureHandle>
{
    std::size_t operator()(const QE::TextureHandle& handle) const noexcept
    {
        return std::hash<std::uint32_t>()(handle.Value);
    }
};