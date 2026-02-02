#pragma once

#include "Utility/RefCounting.h"
#include "RHI_Common.h"

#include <cstdint>
#include <string>

namespace qrhi
{
    struct BufferDesc
    {
        uint64_t size = 0; // in bytes
        uint32_t structStride = 0;
        std::string debugName;
        Format format = Format::UNKNOWN;
        bool canHaveUAVs = false;
        bool canHaveTypedViews = false;
        bool canHaveRawViews = false;
        bool isVertexBuffer = false;
        bool isIndexBuffer = false;
        bool isConstantBuffer = false;
        bool isDrawIndirectArgs = false;
        bool isShaderBindingTable = false;

        // Buffer is dynamic/upload buffer that only is for the current command lists duration
        bool isVolatile = false;

        // Created with no backing memory, to be bound later with bindBufferMemory
        bool isVirtual = false;

        CPUAccessMode cpuAccess = CPUAccessMode::None;

        constexpr BufferDesc& setSize(uint64_t value) { size = value; return *this; }
        constexpr BufferDesc& setStructStride(uint32_t value) { structStride = value; return *this; }
                  BufferDesc& setDebugName(const std::string& value) { debugName = value; return *this; }
        constexpr BufferDesc& setFormat(Format value) { format = value; return *this; }
        constexpr BufferDesc& setCanHaveUAVs(bool value) { canHaveUAVs = value; return *this; }
        constexpr BufferDesc& setCanHaveTypedViews(bool value) { canHaveTypedViews = value; return *this; }
        constexpr BufferDesc& setCanHaveRawViews(bool value) { canHaveRawViews = value; return *this; }
        constexpr BufferDesc& setIsVertexBuffer(bool value) { isVertexBuffer = value; return *this; }
        constexpr BufferDesc& setIsIndexBuffer(bool value) { isIndexBuffer = value; return *this; }
        constexpr BufferDesc& setIsConstantBuffer(bool value) { isConstantBuffer = value; return *this; }
        constexpr BufferDesc& setIsDrawIndirectArgs(bool value) { isDrawIndirectArgs = value; return *this; }
        constexpr BufferDesc& setIsShaderBindingTable(bool value) { isShaderBindingTable = value; return *this; }
        constexpr BufferDesc& setIsVolatile(bool value) { isVolatile = value; return *this; }
        constexpr BufferDesc& setIsVirtual(bool value) { isVirtual = value; return *this; }
        //constexpr BufferDesc& setInitialState(ResourceStates value) { initialState = value; return *this; }
        //constexpr BufferDesc& setKeepInitialState(bool value) { keepInitialState = value; return *this; }
        constexpr BufferDesc& setCpuAccess(CPUAccessMode value) { cpuAccess = value; return *this; }
    };

    struct BufferRange
    {
        uint64_t byteOffset = 0;
        uint64_t byteSize = 0;

        BufferRange() = default;

        BufferRange(uint64_t _byteOffset, uint64_t _byteSize)
            : byteOffset(_byteOffset)
            , byteSize(_byteSize)
        { }

        [[nodiscard]] QUEST_API BufferRange Resolve(const BufferDesc& desc) const;
        [[nodiscard]] constexpr bool isEntireBuffer(const BufferDesc& desc) const { return (byteOffset == 0) && (byteSize == ~0ull || byteSize == desc.size); }
        constexpr bool operator== (const BufferRange& other) const { return byteOffset == other.byteOffset && byteSize == other.byteSize; }

        constexpr BufferRange& setByteOffset(uint64_t value) { byteOffset = value; return *this; }
        constexpr BufferRange& setByteSize(uint64_t value) { byteSize = value; return *this; }
    };

    static const BufferRange EntireBuffer = BufferRange(0, ~0ull);

    class Buffer : public Resource
    {
    public:
        virtual ~Buffer() = default;
        virtual const BufferDesc& GetDesc() const = 0;
        virtual GpuVirtualAddress GetGpuVirtualAddress() const = 0;
    };

    using BufferHandle = Handle_T<Buffer>;
}

namespace std
{
    template<> struct hash<qrhi::BufferRange>
    {
        std::size_t operator()(qrhi::BufferRange const& s) const noexcept
        {
            size_t hash = 0;
            qrhi::hash_combine(hash, s.byteOffset);
            qrhi::hash_combine(hash, s.byteSize);
            return hash;
        }
    };
}