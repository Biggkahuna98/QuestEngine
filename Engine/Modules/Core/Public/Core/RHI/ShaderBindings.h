#pragma once

#include "RHICommon.h"

#include "Shader.h"
#include "Buffer.h"

#include <Core/Containers/StaticVector.h>

namespace Quest
{
    enum class BindingResourceType
    {
        None,
        Texture_SRV,
        Texture_UAV,
        StorageBuffer_SRV,
        StorageBuffer_UAV,
        UniformBuffer,
        DynamicUniformBuffer,
        Sampler,
        PushConstants
    };

    struct BindingLayoutItem
    {
        uint32_t slot;
        BindingResourceType type;
        uint32_t size = 1;

        bool operator==(const BindingLayoutItem& b) const
        {
            return slot == b.slot && type == b.type && size == b.size;
        }
        bool operator!=(const BindingLayoutItem& b) const
        {
            return !(*this == b);
        }

        constexpr BindingLayoutItem& SetSlot(uint32_t value) { slot = value; return *this; }
        constexpr BindingLayoutItem& SetType(BindingResourceType value) { type = value;  return *this; }
        constexpr BindingLayoutItem& SetSize(uint16_t value) { size = value; return *this; }

        #define RESOURCE_ITEM_INIT(TYPE) \
        static BindingLayoutItem TYPE(const uint32_t slot) { \
                BindingLayoutItem item{}; \
                item.slot = slot; \
                item.type = BindingResourceType::TYPE; \
                item.size = 1; \
                return item; } \

        RESOURCE_ITEM_INIT(Texture_SRV)
        RESOURCE_ITEM_INIT(Texture_UAV)
        RESOURCE_ITEM_INIT(StorageBuffer_SRV)
        RESOURCE_ITEM_INIT(StorageBuffer_UAV)
        RESOURCE_ITEM_INIT(UniformBuffer)
        RESOURCE_ITEM_INIT(DynamicUniformBuffer)
        RESOURCE_ITEM_INIT(Sampler)

        static BindingLayoutItem PushConstants(const uint32_t slot, const uint32_t size)
        {
            BindingLayoutItem item{};
            item.slot = slot;
            item.type = BindingResourceType::PushConstants;
            item.size = size;
            return item;
        }
    };

    struct BindingLayoutDesc
    {
        ShaderType shaderVisibility = ShaderType::None;
        std::vector<BindingLayoutItem> bindingItems;

        constexpr BindingLayoutDesc& AddItem(BindingLayoutItem value) { bindingItems.push_back(value); return *this; }
        constexpr BindingLayoutDesc& SetVisibility(ShaderType visibility) { shaderVisibility = visibility; return *this; }
    };

    struct BindlessLayoutDesc
    {
        enum class LayoutType
        {
            Immutable
        };

        ShaderType shaderVisibility = ShaderType::None;
        LayoutType layoutType = LayoutType::Immutable;
        uint32_t firstSlotIndex = 0;
        uint32_t maxCapacity = 0;
        StaticVector<BindingLayoutItem, c_MaxBindlessSlots> bindlessSlots;

        BindlessLayoutDesc& SetVisibility(ShaderType visibility) { shaderVisibility = visibility; return *this; }
        BindlessLayoutDesc& SetLayoutType(LayoutType type) { layoutType = type; return *this; }
        BindlessLayoutDesc& SetFirstSlotIndex(uint32_t idx) { firstSlotIndex = idx; return *this; }
        BindlessLayoutDesc& SetMaxCapacity(uint32_t capacity) { maxCapacity = capacity; return *this; }
        BindlessLayoutDesc& AddBindlessSlot(BindingLayoutItem item) { bindlessSlots.push_back(item); return *this; }
    };

    class BindingLayout : public Resource
    {
    public:
        virtual const BindingLayoutDesc* GetDesc() const = 0; // nullptr if bindless
        virtual const BindlessLayoutDesc* GetBindlessDesc() const = 0; // nullptr if binded
    };

    using BindingLayoutHandle = RefCountPtr<BindingLayout>;

    // Binding sets (descriptor sets)
    struct BindingSetItem
    {
        Resource* resourceHandle = nullptr;

        uint32_t slot = 0;
        uint32_t arrayIndex = 0; // Index in the binding array
        BindingResourceType type = BindingResourceType::None;
        Format format = Format::UNKNOWN;

        union
        {
            BufferRange range; // for buffers
            uint64_t rawData[2];
        };

        bool operator ==(const BindingSetItem& b) const
        {
            return resourceHandle == b.resourceHandle
                && slot == b.slot
                && type == b.type
                && format == b.format;
        }

        bool operator !=(const BindingSetItem& b) const
        {
            return !(*this == b);
        }

        BindingSetItem() {}

        BindingSetItem& SetResource(Resource* res) { resourceHandle = res; return *this; }
        BindingSetItem& SetSlot(uint32_t s) { slot = s; return *this; }
        BindingSetItem& SetType(BindingResourceType t) { type = t; return *this; }
        BindingSetItem& SetFormat(Format f) { format = f; return *this; }

        static BindingSetItem None(uint32_t slot = 0)
        {
            BindingSetItem item{};

            item.slot = slot;
            item.arrayIndex = 0;
            item.type = BindingResourceType::None;
            item.resourceHandle = nullptr;
            item.format = Format::UNKNOWN;
            item.rawData[0] = 0;
            item.rawData[1] = 0;

            return item;
        }

        static BindingSetItem UniformBuffer(uint32_t slot, Buffer* buffer, BufferRange range = EntireBuffer)
        {
            bool isVolatile = buffer && buffer->GetDesc().isVolatile;
            BindingSetItem item{};

            item.slot = slot;
            item.arrayIndex = 0;
            item.type = isVolatile ? BindingResourceType::DynamicUniformBuffer : BindingResourceType::UniformBuffer;
            item.resourceHandle = buffer;
            item.format = Format::UNKNOWN;
            item.range = range;

            return item;
        }
    };

    struct BindingSetDesc
    {
        std::vector<BindingSetItem> bindingSetItems;

        bool operator ==(const BindingSetDesc& b) const
        {
            if (bindingSetItems.size() != b.bindingSetItems.size())
                return false;

            for (size_t i = 0; i < bindingSetItems.size(); ++i)
            {
                if (bindingSetItems[i] != b.bindingSetItems[i])
                    return false;
            }

            return true;
        }

        bool operator !=(const BindingSetDesc& b) const
        {
            return !(*this == b);
        }

        BindingSetDesc& AddItem(const BindingSetItem& item) { bindingSetItems.push_back(item); return *this; }
    };

    class BindingSet : public Resource
    {
    public:
        virtual const BindingSetDesc* GetDesc() const = 0;
        virtual BindingLayout* GetLayout() = 0;
    };

    using BindingSetHandle = RefCountPtr<BindingSet>;
}