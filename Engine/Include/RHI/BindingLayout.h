#pragma once

#include "RHI_Common.h"
#include "Shader.h"
#include "Texture.h"
#include "Buffer.h"
#include "Sampler.h"

#include <vector>

namespace qrhi
{
    // Underlying resource type in the binding
    enum class BindingResourceType : uint8_t
    {
        None,
        Texture_SRV,
        Texture_UAV,
        TypedBuffer_SRV,
        TypedBuffer_UAV,
        StructuredBuffer_SRV,
        StructuredBuffer_UAV,
        RawBuffer_SRV,
        RawBuffer_UAV,
        ConstantBuffer,
        VolatileConstantBuffer,
        Sampler,
        PushConstants,

        Count
    };

    struct BindingLayoutItem
    {
        uint32_t slot;

        BindingResourceType type = BindingResourceType::None;
        bool isUsed = true;

        // Byte size of push constant
        // Size (quantity) of array size (1 for single, more for multiple)
        uint16_t size = 1;

        bool operator ==(const BindingLayoutItem& rhs) const
        {
            return slot == rhs.slot
                && type == rhs.type
                && size == rhs.size;
        }

        bool operator !=(const BindingLayoutItem& rhs) const { return !(*this == rhs); }

        constexpr BindingLayoutItem& setSlot(uint32_t value) { slot = value; return *this; }
        constexpr BindingLayoutItem& setType(BindingResourceType value) { type = value; return *this; }
        constexpr BindingLayoutItem& setSize(uint16_t value) { size = value; return *this; }

        // Helpers for item initialization
        static BindingLayoutItem Texture_SRV(const uint32_t slot)
        {
            BindingLayoutItem item{};
            item.slot = slot;
            item.type = BindingResourceType::Texture_SRV;
            item.size = 1;
            return item;
        }

        static BindingLayoutItem Texture_UAV(const uint32_t slot)
        {
            BindingLayoutItem item{};
            item.slot = slot;
            item.type = BindingResourceType::Texture_UAV;
            item.size = 1;
            return item;
        }

        static BindingLayoutItem TypedBuffer_SRV(const uint32_t slot)
        {
            BindingLayoutItem item{};
            item.slot = slot;
            item.type = BindingResourceType::TypedBuffer_SRV;
            item.size = 1;
            return item;
        }

        static BindingLayoutItem TypedBuffer_UAV(const uint32_t slot)
        {
            BindingLayoutItem item{};
            item.slot = slot;
            item.type = BindingResourceType::TypedBuffer_UAV;
            item.size = 1;
            return item;
        }

        static BindingLayoutItem StructuredBuffer_SRV(const uint32_t slot)
        {
            BindingLayoutItem item{};
            item.slot = slot;
            item.type = BindingResourceType::StructuredBuffer_SRV;
            item.size = 1;
            return item;
        }

        static BindingLayoutItem StructuredBuffer_UAV(const uint32_t slot)
        {
            BindingLayoutItem item{};
            item.slot = slot;
            item.type = BindingResourceType::StructuredBuffer_UAV;
            item.size = 1;
            return item;
        }

        static BindingLayoutItem RawBuffer_SRV(const uint32_t slot)
        {
            BindingLayoutItem item{};
            item.slot = slot;
            item.type = BindingResourceType::RawBuffer_SRV;
            item.size = 1;
            return item;
        }

        static BindingLayoutItem RawBuffer_UAV(const uint32_t slot)
        {
            BindingLayoutItem item{};
            item.slot = slot;
            item.type = BindingResourceType::RawBuffer_UAV;
            item.size = 1;
            return item;
        }

        static BindingLayoutItem ConstantBuffer(const uint32_t slot)
        {
            BindingLayoutItem item{};
            item.slot = slot;
            item.type = BindingResourceType::ConstantBuffer;
            item.size = 1;
            return item;
        }

        static BindingLayoutItem VolatileConstantBuffer(const uint32_t slot)
        {
            BindingLayoutItem item{};
            item.slot = slot;
            item.type = BindingResourceType::VolatileConstantBuffer;
            item.size = 1;
            return item;
        }

        static BindingLayoutItem Sampler(const uint32_t slot)
        {
            BindingLayoutItem item{};
            item.slot = slot;
            item.type = BindingResourceType::Sampler;
            item.size = 1;
            return item;
        }

        static BindingLayoutItem PushConstants(const uint32_t slot, const uint16_t size)
        {
            BindingLayoutItem item{};
            item.slot = slot;
            item.type = BindingResourceType::PushConstants;
            item.size = size;
            return item;
        }
    };

    static_assert(sizeof(BindingLayoutItem) == 8, "sizeof(BindingLayoutItem) should be 8 bytes for good alignment");

    struct BindingLayoutDesc
    {
        ShaderType visibility = ShaderType::None;

        std::vector<BindingLayoutItem> items;

        BindingLayoutDesc& setVisibility(ShaderType value) { visibility = value; return *this; }
        BindingLayoutDesc& addItem(const BindingLayoutItem& item) { items.push_back(item); return *this; }
    };

    struct BindlessLayoutDesc
    {
        ShaderType visibility = ShaderType::None;
        uint32_t firstSlot = 0;
        uint32_t numSlots = 0;

        // TODO: make sure this has a capacity of c_MaxBindlessSlots
        std::vector<BindingLayoutItem> items;

        BindlessLayoutDesc& setVisibility(ShaderType value) { visibility = value; return *this; }
        BindlessLayoutDesc& setFirstSlot(uint32_t value) { firstSlot = value; return *this; }
        BindlessLayoutDesc& setNumSlots(uint32_t value) { numSlots = value; return *this; }
        BindlessLayoutDesc& addItem(const BindingLayoutItem& item) { items.push_back(item); return *this; }
    };

    class BindingLayout : public Resource
    {
    public:
        [[nodiscard]] virtual const BindingLayoutDesc* GetDesc() const = 0; // nullptr if it is a bindless layout
        [[nodiscard]] virtual const BindlessLayoutDesc* GetBindlessDesc() const = 0; // nullptr if it is a binding layout
    };

    using BindingLayoutHandle = Quest::RefCountPtr<BindingLayout>;

    struct BindingSetItem
    {
        BindingSetItem() {}
        Resource* resourceHandle = nullptr;
        uint32_t slot = 0;
        uint32_t arrayIndex = 0;
        BindingResourceType type = BindingResourceType::None;
        TextureDimension dimension = TextureDimension::Texture2D;
        Format format = Format::UNKNOWN;

        union
        {
            TextureSubresourceSet subresources;
            BufferRange bufferRange;
            uint64_t rawData[2];
        };

        static_assert(sizeof(TextureSubresourceSet) == 16, "sizeof(TextureSubresourceSet) is supposed to be 16 bytes");
        static_assert(sizeof(BufferRange) == 16, "sizeof(BufferRange) is supposed to be 16 bytes");

        bool operator ==(const BindingSetItem& b) const
        {
            return resourceHandle == b.resourceHandle
                && slot == b.slot
                && type == b.type
                && dimension == b.dimension
                && format == b.format
                && rawData[0] == b.rawData[0]
                && rawData[1] == b.rawData[1];
        }

        bool operator !=(const BindingSetItem& b) const
        {
            return !(*this == b);
        }

        static BindingSetItem None(uint32_t slot = 0)
        {
            BindingSetItem result;
            result.slot = slot;
            result.arrayIndex = 0;
            result.type = BindingResourceType::None;
            result.resourceHandle = nullptr;
            result.format = Format::UNKNOWN;
            result.dimension = TextureDimension::Unknown;
            result.rawData[0] = 0;
            result.rawData[1] = 0;
            return result;
        }

        static BindingSetItem Texture_SRV(uint32_t slot, Texture* texture, Format format = Format::UNKNOWN,
            TextureSubresourceSet subresources = AllSubresources, TextureDimension dimension = TextureDimension::Unknown)
        {
            BindingSetItem result;
            result.slot = slot;
            result.arrayIndex = 0;
            result.type = BindingResourceType::Texture_SRV;
            result.resourceHandle = texture;
            result.format = format;
            result.dimension = dimension;
            result.subresources = subresources;
            return result;
        }

        static BindingSetItem Texture_UAV(uint32_t slot, Texture* texture, Format format = Format::UNKNOWN,
            TextureSubresourceSet subresources = TextureSubresourceSet(0, 1, 0, TextureSubresourceSet::AllArraySlices),
            TextureDimension dimension = TextureDimension::Unknown)
        {
            BindingSetItem result;
            result.slot = slot;
            result.arrayIndex = 0;
            result.type = BindingResourceType::Texture_UAV;
            result.resourceHandle = texture;
            result.format = format;
            result.dimension = dimension;
            result.subresources = subresources;
            return result;
        }

        static BindingSetItem TypedBuffer_SRV(uint32_t slot, Buffer* buffer, Format format = Format::UNKNOWN, BufferRange range = EntireBuffer)
        {
            BindingSetItem result;
            result.slot = slot;
            result.arrayIndex = 0;
            result.type = BindingResourceType::TypedBuffer_SRV;
            result.resourceHandle = buffer;
            result.format = format;
            result.dimension = TextureDimension::Unknown;
            result.bufferRange = range;
            return result;
        }

        static BindingSetItem TypedBuffer_UAV(uint32_t slot, Buffer* buffer, Format format = Format::UNKNOWN, BufferRange range = EntireBuffer)
        {
            BindingSetItem result;
            result.slot = slot;
            result.arrayIndex = 0;
            result.type = BindingResourceType::TypedBuffer_UAV;
            result.resourceHandle = buffer;
            result.format = format;
            result.dimension = TextureDimension::Unknown;
            result.bufferRange = range;
            return result;
        }

        static BindingSetItem ConstantBuffer(uint32_t slot, Buffer* buffer, BufferRange range = EntireBuffer)
        {
            bool isVolatile = buffer && buffer->GetDesc().isVolatile;

            BindingSetItem result;
            result.slot = slot;
            result.arrayIndex = 0;
            result.type = isVolatile ? BindingResourceType::VolatileConstantBuffer : BindingResourceType::ConstantBuffer;
            result.resourceHandle = buffer;
            result.format = Format::UNKNOWN;
            result.dimension = TextureDimension::Unknown;
            result.bufferRange = range;
            return result;
        }

        static BindingSetItem Sampler(uint32_t slot, Sampler* sampler)
        {
            BindingSetItem result;
            result.slot = slot;
            result.arrayIndex = 0;
            result.type = BindingResourceType::Sampler;
            result.resourceHandle = sampler;
            result.format = Format::UNKNOWN;
            result.dimension = TextureDimension::Unknown;
            result.rawData[0] = 0;
            result.rawData[1] = 0;
            return result;
        }

        static BindingSetItem StructuredBuffer_SRV(uint32_t slot, Buffer* buffer, Format format = Format::UNKNOWN, BufferRange range = EntireBuffer)
        {
            BindingSetItem result;
            result.slot = slot;
            result.arrayIndex = 0;
            result.type = BindingResourceType::StructuredBuffer_SRV;
            result.resourceHandle = buffer;
            result.format = format;
            result.dimension = TextureDimension::Unknown;
            result.bufferRange = range;
            return result;
        }

        static BindingSetItem StructuredBuffer_UAV(uint32_t slot, Buffer* buffer, Format format = Format::UNKNOWN, BufferRange range = EntireBuffer)
        {
            BindingSetItem result;
            result.slot = slot;
            result.arrayIndex = 0;
            result.type = BindingResourceType::StructuredBuffer_UAV;
            result.resourceHandle = buffer;
            result.format = format;
            result.dimension = TextureDimension::Unknown;
            result.bufferRange = range;
            return result;
        }

        static BindingSetItem RawBuffer_SRV(uint32_t slot, Buffer* buffer, BufferRange range = EntireBuffer)
        {
            BindingSetItem result;
            result.slot = slot;
            result.arrayIndex = 0;
            result.type = BindingResourceType::RawBuffer_SRV;
            result.resourceHandle = buffer;
            result.format = Format::UNKNOWN;
            result.dimension = TextureDimension::Unknown;
            result.bufferRange = range;
            return result;
        }

        static BindingSetItem RawBuffer_UAV(uint32_t slot, Buffer* buffer, BufferRange range = EntireBuffer)
        {
            BindingSetItem result;
            result.slot = slot;
            result.arrayIndex = 0;
            result.type = BindingResourceType::RawBuffer_UAV;
            result.resourceHandle = buffer;
            result.format = Format::UNKNOWN;
            result.dimension = TextureDimension::Unknown;
            result.bufferRange = range;
            return result;
        }

        static BindingSetItem PushConstants(uint32_t slot, uint32_t byteSize)
        {
            BindingSetItem result;
            result.slot = slot;
            result.arrayIndex = 0;
            result.type = BindingResourceType::PushConstants;
            result.resourceHandle = nullptr;
            result.format = Format::UNKNOWN;
            result.dimension = TextureDimension::Unknown;
            result.bufferRange.byteOffset = 0;
            result.bufferRange.byteSize = byteSize;
            return result;
        }

        BindingSetItem& setArrayIndex(uint32_t value) { arrayIndex = value; return *this; }
        BindingSetItem& setFormat(Format value) { format = value; return *this; }
        BindingSetItem& setDimension(TextureDimension value) { dimension = value; return *this; }
        BindingSetItem& setSubresources(TextureSubresourceSet value) { subresources = value; return *this; }
        BindingSetItem& setRange(BufferRange value) { range = value; return *this; }
    };

    static_assert(sizeof(BindingSetItem) == 40, "sizeof(BindingSetItem) is supposed to be 40 bytes");

    struct BindingSetDesc
    {
        std::vector<BindingSetItem> items;

        bool operator ==(const BindingSetDesc& b) const
        {
            if (items.size() != b.items.size())
                return false;

            for (size_t i = 0; i < items.size(); ++i)
            {
                if (items[i] != b.items[i])
                    return false;
            }

            return true;
        }

        bool operator !=(const BindingSetDesc& b) const
        {
            return !(*this == b);
        }

        BindingSetDesc& addItem(const BindingSetItem& value) { items.push_back(value); return *this; }
    };

    class BindingSet : public Resource
    {
    public:
        [[nodiscard]] virtual const BindingSetDesc* GetDesc() const = 0;
        [[nodiscard]] virtual BindingLayout* GetLayout() const = 0;
    };

    using BindingSetHandle = Quest::RefCountPtr<BindingSet>;
}

namespace std
{
    template<> struct hash<qrhi::BindingSetItem>
    {
        std::size_t operator()(qrhi::BindingSetItem const& s) const noexcept
        {
            size_t value = 0;
            qrhi::hash_combine(value, s.resourceHandle);
            qrhi::hash_combine(value, s.slot);
            qrhi::hash_combine(value, s.type);
            qrhi::hash_combine(value, s.dimension);
            qrhi::hash_combine(value, s.format);
            qrhi::hash_combine(value, s.rawData[0]);
            qrhi::hash_combine(value, s.rawData[1]);
            return value;
        }
    };

    template<> struct hash<qrhi::BindingSetDesc>
    {
        std::size_t operator()(qrhi::BindingSetDesc const& s) const noexcept
        {
            size_t value = 0;
            for (const auto& item : s.items)
                qrhi::hash_combine(value, item);
            return value;
        }
    };
}
