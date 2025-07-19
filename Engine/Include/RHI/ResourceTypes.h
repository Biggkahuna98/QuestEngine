#pragma once

#include "Core/Core.h"
#include "RHISettings.h"
#include <array>
#include <vector>
#include <optional>
#include <glm/glm.hpp>

namespace QE
{
    // Invalid handle is 0xFFFFFFFF
    constexpr int InvalidHandleValue = -1;

    // Vertex - uv_x and uv_y are tex coords
    struct QUEST_API Vertex
    {
        alignas(16) glm::vec3 Position;
        alignas(16) glm::vec3 Normal;
        alignas(16) glm::vec2 TexCoords;
        alignas(16) glm::vec4 Color;
    };

    // Matters to RHIs
    struct QUEST_API ModelViewProjection
    {
        glm::mat4 Model;
        glm::mat4 View;
        glm::mat4 Projection;
    };

    // Handles
    struct QUEST_API BufferHandle
    {
        std::uint32_t Value;

        BufferHandle(std::uint32_t value = -1) : Value(value) {}
        bool operator==(const BufferHandle& other) const
        {
            return other.Value == Value;
        }
    };

    struct QUEST_API TextureHandle
    {
        std::uint32_t Value;

        TextureHandle(std::uint32_t value = -1) : Value(value) {}
        bool operator==(const TextureHandle& other) const
        {
            return other.Value == Value;
        }
    };

    struct QUEST_API MeshHandle
    {
        std::uint32_t Value;

        MeshHandle(std::uint32_t value = -1) : Value(value) {}
        bool operator==(const MeshHandle& other) const
        {
            return other.Value == Value;
        }
    };

    struct QUEST_API ShaderHandle
    {
        std::uint32_t Value;

        ShaderHandle(std::uint32_t value = -1) : Value(value) {}
        bool operator==(const ShaderHandle& other) const
        {
            return other.Value == Value;
        }
    };

    struct QUEST_API PipelineHandle
    {
        std::uint32_t Value;

        PipelineHandle(std::uint32_t value = -1) : Value(value) {}
        bool operator==(const PipelineHandle& other) const
        {
            return other.Value == Value;
        }
    };

    struct QUEST_API RenderpassHandle
    {
        std::uint32_t Value;

        RenderpassHandle(std::uint32_t value = -1) : Value(value) {}
        bool operator==(const RenderpassHandle& other) const
        {
            return other.Value == Value;
        }
    };

    // Types depending on the above definitions
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
        std::uint32_t ImageWidth;
        std::uint32_t ImageHeight;
        std::uint32_t ImageDepth = 1;
    };

    struct QUEST_API ShaderDescription
    {
        std::string SourcePath;
        ShaderHandle Handle = InvalidHandleValue;
        ShaderStage Stage;
    };

    struct QUEST_API PipelineDescription
    {
        std::vector<ShaderDescription> Shaders;
        PipelineTopology Topology;
        bool PrimitiveRestart;
        PolygonMode PolygonMode;
        CullMode CullMode;
        WindingOrder WindingOrder;
        bool DepthTest;
        bool DepthWrite;
        DepthCompareOp DepthCompareOp;
        bool Blending;
        BlendingType BlendingType;
    };

    struct QUEST_API RenderpassDescription
    {
        PipelineHandle Pipeline;
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

template<>
struct std::hash<QE::ShaderHandle>
{
    std::size_t operator()(const QE::ShaderHandle& handle) const noexcept
    {
        return std::hash<std::uint32_t>()(handle.Value);
    }
};

template<>
struct std::hash<QE::PipelineHandle>
{
    std::size_t operator()(const QE::PipelineHandle& handle) const noexcept
    {
        return std::hash<std::uint32_t>()(handle.Value);
    }
};

template<>
struct std::hash<QE::RenderpassHandle>
{
    std::size_t operator()(const QE::RenderpassHandle& handle) const noexcept
    {
        return std::hash<std::uint32_t>()(handle.Value);
    }
};