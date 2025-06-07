#pragma once
#include <span>

#include "Core/Core.h"
#include "Core/Containers/RawBuffer.h"
#include "RHISettings.h"
#include <vector>
#include <glm/glm.hpp>

namespace QE
{
    // Vertex - uv_x and uv_y are tex coords
    struct QUEST_API Vertex
    {
        glm::vec3 Position;
        float uv_x = 0;
        glm::vec3 Normal;
        float uv_y = 0;
        glm::vec4 Color;
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

    struct QUEST_API MeshHandle
    {
        std::uint32_t Value;
        bool operator==(const MeshHandle& other) const
        {
            return other.Value == Value;
        }
    };

    // Descriptions
    struct QUEST_API BufferDescription
    {
        BufferType Type;
        BufferUsage Usage = BufferUsage::Default;
        std::vector<std::uint8_t> Data;
        std::size_t DataSize = 0;
        std::size_t Count = 0;
    };

    struct QUEST_API TextureDescription
    {
        std::vector<std::uint8_t> Data;
        std::size_t ImageSize;
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

template<>
struct std::hash<QE::MeshHandle>
{
    std::size_t operator()(const QE::MeshHandle& handle) const noexcept
    {
        return std::hash<std::uint32_t>()(handle.Value);
    }
};