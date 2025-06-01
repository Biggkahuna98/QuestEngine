#pragma once
#include "Core/Core.h"

namespace QE
{
    enum class QUEST_API PreferedGPUType : std::uint8_t
    {
        Discrete,
        Integrated
    };

    enum class QUEST_API ShaderStage : std::uint8_t
    {
        Vertex,
        Fragment,
        Compute
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
}