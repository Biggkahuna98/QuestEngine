#pragma once

namespace QE
{
    enum class PreferedGPUType : std::uint8_t
    {
        Discrete,
        Integrated
    };

    enum class ShaderStage : std::uint8_t
    {
        Vertex,
        Fragment,
        Compute
    };

    enum class BufferType : std::uint8_t
    {
        Vertex,
        Index
    };
}