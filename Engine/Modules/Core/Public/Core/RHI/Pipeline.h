#pragma once

namespace Quest
{
    enum class PrimitiveType
    {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
        TriangleFan,
        TriangleListWithAdjacency,
        TriangleStripWithAdjacency,
        PatchList
    };

    enum class FillMode
    {
        Solid,
        Wireframe
    };

    enum class CullMode
    {
        Back,
        Front,
        None
    };
}