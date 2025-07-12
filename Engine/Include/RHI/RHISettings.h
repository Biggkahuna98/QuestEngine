#pragma once
#include "Core/Core.h"

namespace QE
{
    enum class QUEST_API PreferedGPUType : std::uint8_t
    {
        Discrete,
        Integrated
    };

    enum class QUEST_API BufferType : std::uint8_t
    {
        Vertex,
        Index,
        Uniform,
        Transfer
    };

    enum class QUEST_API BufferUsage : std::uint8_t
    {
        Default, // GPU read/write, no cpu access
        Upload,
        Readback
    };

    enum class QUEST_API ShaderStage : std::uint8_t
    {
        Vertex,
        Fragment,
        Compute,
        DeduceType // To be used later
    };

    enum class QUEST_API PipelineTopology : std::uint8_t
    {
        PointList,
        LineList,
        LineStrip,
        LineListAdjacency,
        LineStripAdjacency,
        TriangleList,
        TriangleStrip,
        TriangleFan,
        TriangleListAdjacency,
        TriangleStripAdjacency,
        PatchList
    };

    enum class QUEST_API PolygonMode : std::uint8_t
    {
        Point,
        Line, // Wireframe
        Fill // Normal
    };

    enum class QUEST_API CullMode : std::uint8_t
    {
        None,
        Front,
        Back,
        FrontBack // All triangles are discarded
    };

    enum class QUEST_API WindingOrder : std::uint8_t
    {
        Clockwise,
        CounterClockwise
    };

    enum class QUEST_API DepthCompareOp
    {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always
    };

    enum class QUEST_API BlendingType
    {
        None,
        Alpha,
        Add
    };
}