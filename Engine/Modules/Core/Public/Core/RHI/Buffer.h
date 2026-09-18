#pragma once

#include "RHICommon.h"

#include <cstdint>

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
        uint64_t sizeInBytes{};
        uint64_t size{};
        uint64_t stride{};

        bool isVolatile = false;
    };

    struct BufferRange
    {
        uint64_t offsetInBytes = 0;
        uint64_t sizeInBytes = 0;

        BufferRange() = default;

        BufferRange(uint64_t offset, uint64_t size)
            : offsetInBytes(offset), sizeInBytes(size)
        {}

        BufferRange Resolve(const BufferDesc& desc) const;
        constexpr bool IsEntireBuffer(const BufferDesc& desc) const { return (offsetInBytes == 0) && (sizeInBytes == ~0ull) || sizeInBytes == desc.sizeInBytes; }
        constexpr bool operator==(const BufferRange& other) const { return offsetInBytes == other.offsetInBytes && sizeInBytes == other.sizeInBytes; }

        constexpr BufferRange& SetOffset(uint64_t val) { offsetInBytes = val; return *this; }
        constexpr BufferRange& SetSize(uint64_t val) { sizeInBytes = val; return *this; }
    };

    static const BufferRange EntireBuffer = BufferRange(0, ~0ull);

    class Buffer : public Resource
    {
    public:
        virtual const BufferDesc& GetDesc() const = 0;
    };

    using BufferHandle = RefCountPtr<Buffer>;
}