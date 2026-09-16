#pragma once
#include "Core/GenericHandle.h"

namespace Quest
{
    enum class BufferType
    {
        Vertex,
        Index,
        Uniform
    };

    struct BufferDesc
    {
        BufferType type;
        uint64_t sizeInBytes;
        uint64_t size;
        uint64_t stride;

        bool isVolatile = false;
    };

    using BufferHandle = Handle<struct BufferTag>;
}