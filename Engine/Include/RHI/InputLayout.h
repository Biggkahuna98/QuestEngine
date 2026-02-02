#pragma once

#include "Utility/RefCounting.h"

#include "RHI_Common.h"
#include <cstdint>
#include <string>

namespace qrhi
{
    struct VertexAttributeDesc
    {
        std::string name;
        Format format = Format::UNKNOWN;
        uint32_t arraySize = 1;
        uint32_t bufferIndex = 0;
        uint32_t offset = 0;
        // note: for most APIs, all strides for a given bufferIndex must be identical
        uint32_t elementStride = 0;
        bool isInstanced = false;

        VertexAttributeDesc& setName(const std::string& value) { name = value; return *this; }
        constexpr VertexAttributeDesc& setFormat(Format value) { format = value; return *this; }
        constexpr VertexAttributeDesc& setArraySize(uint32_t value) { arraySize = value; return *this; }
        constexpr VertexAttributeDesc& setBufferIndex(uint32_t value) { bufferIndex = value; return *this; }
        constexpr VertexAttributeDesc& setOffset(uint32_t value) { offset = value; return *this; }
        constexpr VertexAttributeDesc& setElementStride(uint32_t value) { elementStride = value; return *this; }
        constexpr VertexAttributeDesc& setIsInstanced(bool value) { isInstanced = value; return *this; }
    };

    class InputLayout : public Resource
    {
    public:
        [[nodiscard]] virtual uint32_t GetNumAttributes() const = 0;
        [[nodiscard]] virtual const VertexAttributeDesc* GetAttributeDesc(uint32_t index) const = 0;
    };

    using InputLayoutHandle = Handle_T<InputLayout>;
}
