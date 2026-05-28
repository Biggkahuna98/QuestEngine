#pragma once

#include <string>
#include <string_view>

#include "Core/Core.h"
#include "Utility/OpaqueObject.h"
#include "Utility/RefCounting.h"
#include "Utility/StaticVector.h"

#include <glm/glm.hpp>

// Enum flag operators
#define QRHI_ENUM_CLASS_FLAG_OPERATORS(T) \
    inline T operator | (T a, T b) { return T(uint32_t(a) | uint32_t(b)); } \
    inline T operator & (T a, T b) { return T(uint32_t(a) & uint32_t(b)); } \
    inline T operator ~ (T a) { return T(~uint32_t(a)); } \
    inline T operator &= (T a, T b) { return a = a & b; } \
    inline T operator |= (T a, T b) { return a = a | b; } \
    inline bool operator !(T a) { return uint32_t(a) == 0; } \
    inline bool operator ==(T a, uint32_t b) { return uint32_t(a) == b; } \
    inline bool operator !=(T a, uint32_t b) { return uint32_t(a) != b; }

namespace qrhi
{
    // Constants
    static constexpr uint32_t c_MaxBindlessSlots = 16;
    static constexpr uint32_t c_MaxBindingLayouts = 8;
    static constexpr uint32_t c_MaxDynamicUniformBuffersPerLayout = 6;

    enum class API
    {
        None, // no-op, doesn't do anything
        Vulkan // default
    };

    enum class Format : uint8_t
    {
        UNKNOWN,

        // Single/Dual Channel
        R8_UNORM,
        R8_SNORM,
        RG8_UNORM,
        RG8_SNORM,
        R16_FLOAT,
        RG16_FLOAT,
        RG16_UNORM,
        R32_UINT,
        R32_SINT,
        R32_FLOAT,
        RG32_FLOAT,

        // Color Render Targets & Textures
        RGBA8_UNORM,
        RGBA8_SNORM,
        SRGBA8_UNORM,
        BGRA8_UNORM,
        SBGRA8_UNORM,
        R10G10B10A2_UNORM,
        R11G11B10_FLOAT,
        RGBA16_FLOAT,
        RGBA16_UNORM,
        RGBA32_FLOAT,

        // Integer / Compute
        R8_UINT,
        R8_SINT,
        RG8_UINT,
        R16_UINT,
        R16_SINT,
        RG16_UINT,
        RG16_SINT,
        RGBA8_UINT,
        RGBA8_SINT,
        RGBA16_UINT,
        RGBA16_SINT,
        RGBA32_UINT,
        RGBA32_SINT,
        RG32_UINT,
        RG32_SINT,

        // Depth / Stencil
        D16,
        D24S8,
        D32,
        D32S8,
        X24G8_UINT,
        X32G8_UINT,

        // Block Compression
        BC1_UNORM,
        BC1_UNORM_SRGB,
        BC3_UNORM,
        BC3_UNORM_SRGB,
        BC4_UNORM,
        BC4_SNORM,
        BC5_UNORM,
        BC5_SNORM,
        BC6H_UFLOAT,
        BC6H_SFLOAT,
        BC7_UNORM,
        BC7_UNORM_SRGB,

        COUNT,
    };

    class MessageCallback
    {
    public:
        enum class Severity
        {
            Info,
            Warning,
            Error
        };
        virtual ~MessageCallback() = default;
        virtual void Message(Severity level, std::string_view message) = 0;
        virtual void Info(std::string_view message) { Message(Severity::Info, message); }
        virtual void Warning(std::string_view message) { Message(Severity::Warning, message); }
        virtual void Error(std::string_view message) { Message(Severity::Error, message); }
    };

    struct Color
    {
        float r, g, b, a;

        Color() : r(0.f), g(0.f), b(0.f), a(0.f) { }
        Color(float c) : r(c), g(c), b(c), a(c) { }
        Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) { }

        bool operator ==(const Color& _b) const { return r == _b.r && g == _b.g && b == _b.b && a == _b.a; }
        bool operator !=(const Color& _b) const { return !(*this == _b); }
    };

    struct Vertex
    {
        glm::vec2 position;
        glm::vec3 color;
    };

    class Resource : public Quest::RefCounted
    {
    public:
        virtual ~Resource() = default;

        // The API specific implementation, i.e. VulkanBuffer which gives access to the underlying types
        virtual Quest::OpaqueObject GetNativeType() = 0;
        virtual void SetDebugName(const std::string& name) = 0;
    };

    using ResourceHandle = Quest::RefCountPtr<Resource>;

    // Buffer
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
        constexpr bool operator ==(const BufferRange& other) const { return offsetInBytes == other.offsetInBytes && sizeInBytes == other.sizeInBytes; }

        constexpr BufferRange& SetOffset(uint64_t val) { offsetInBytes = val; return *this; }
        constexpr BufferRange& SetSize(uint64_t val) { sizeInBytes = val; return *this; }
    };

    static const BufferRange EntireBuffer = BufferRange(0, ~0ull);

    class Buffer : public Resource
    {
    public:
        virtual ~Buffer() = default;
        virtual const BufferDesc& GetDesc() const = 0;
    };

    using BufferHandle = Quest::RefCountPtr<Buffer>;

    // Texture
    struct TextureDesc
    {

    };

    class Texture : public Resource
    {
    public:
        virtual ~Texture() = default;
        virtual const TextureDesc& GetDesc() const = 0;
    };

    using TextureHandle = Quest::RefCountPtr<Texture>;

    // Shader
    enum class ShaderType : uint16_t
    {
        None,

        Vertex,
        Fragment,
        Compute,

        AllGraphics,
        All
    };
    QRHI_ENUM_CLASS_FLAG_OPERATORS(ShaderType);

    enum class ShaderLanguage : uint8_t
    {
        Slang, // can be compiled on the fly
        SPIRV // compiled bytecode
    };

    struct ShaderDesc
    {
        std::string name;
        std::string entry_point = "main";
        ShaderType type = ShaderType::Vertex;
        ShaderLanguage language = ShaderLanguage::Slang;
    };

    class Shader : public Resource
    {
    public:
        virtual ~Shader() = default;
        virtual const ShaderDesc& GetDesc() const = 0;
    };

    using ShaderHandle = Quest::RefCountPtr<Shader>;

    // Shader binding items (descriptors)
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
        Quest::StaticVector<BindingLayoutItem, c_MaxBindlessSlots> bindlessSlots;

        BindlessLayoutDesc& SetVisibility(ShaderType visibility) { shaderVisibility = visibility; return *this; }
        BindlessLayoutDesc& SetLayoutType(LayoutType type) { layoutType = type; return *this; }
        BindlessLayoutDesc& SetFirstSlotIndex(uint32_t idx) { firstSlotIndex = idx; return *this; }
        BindlessLayoutDesc& SetMaxCapacity(uint32_t capacity) { maxCapacity = capacity; return *this; }
        BindlessLayoutDesc& AddBindlessSlot(BindingLayoutItem item) { bindlessSlots.push_back(item); return *this; }
    };

    class BindingLayout : public Resource
    {
    public:
        virtual ~BindingLayout() = default;
        virtual const BindingLayoutDesc* GetDesc() const = 0; // nullptr if bindless
        virtual const BindlessLayoutDesc* GetBindlessDesc() const = 0; // nullptr if binded
    };

    using BindingLayoutHandle = Quest::RefCountPtr<BindingLayout>;

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
        virtual ~BindingSet() = default;
        virtual const BindingSetDesc* GetDesc() const = 0;
        virtual BindingLayout* GetLayout() = 0;
    };

    using BindingSetHandle = Quest::RefCountPtr<BindingSet>;

    // Pipeline
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

    using BindingLayoutVector = Quest::StaticVector<BindingLayoutHandle, c_MaxBindingLayouts>;

    struct GraphicsPipelineDesc
    {
        PrimitiveType primitiveType = PrimitiveType::TriangleList;
        FillMode fillMode = FillMode::Solid;

        ShaderHandle vertexShader;
        ShaderHandle fragmentShader;

        bool uberShader = true;

        BindingLayoutVector bindingLayouts;

        GraphicsPipelineDesc& SetPrimitiveType(PrimitiveType type) { primitiveType = type; return *this; }
        GraphicsPipelineDesc& SetFillMode(FillMode f) { fillMode = f; return *this; }
        GraphicsPipelineDesc& SetVertexShader(ShaderHandle shader) { vertexShader = shader; return *this; }
        GraphicsPipelineDesc& SetFragmentShader(ShaderHandle shader) { fragmentShader = shader; return *this; }
        GraphicsPipelineDesc& AddBindingLayout(BindingLayout* layout) { bindingLayouts.push_back(layout); return *this; }
    };

    class GraphicsPipeline : public Resource
    {
    public:
        virtual ~GraphicsPipeline() = default;
        virtual const GraphicsPipelineDesc& GetDesc() const = 0;
    };

    using GraphicsPipelineHandle = Quest::RefCountPtr<GraphicsPipeline>;

    struct ComputePipelineDesc
    {
        ShaderHandle computeShader;
        BindingLayoutVector bindingLayouts;

        ComputePipelineDesc& SetComputeShader(ShaderHandle shader) { computeShader = shader; return *this; }
        ComputePipelineDesc& AddBindingLayout(BindingLayout* layout) { bindingLayouts.push_back(layout); return *this; }
    };

    class ComputePipeline : public Resource
    {
    public:
        virtual ~ComputePipeline() = default;
        virtual const ComputePipelineDesc& GetDesc() const = 0;
    };

    using ComputePipelineHandle = Quest::RefCountPtr<ComputePipeline>;

    // Draw States
    struct GraphicsState
    {
        GraphicsPipeline* pipeline = nullptr;
        BufferHandle vertexBuffer = nullptr;
        BufferHandle indexBuffer = nullptr;

        BindingSetHandle bindingSet = nullptr;
    };

    struct ComputeState
    {
        ComputePipeline* pipeline = nullptr;
    };

    struct DrawArguments
    {
        uint32_t vertexCount = 0;
        uint32_t instanceCount = 1;
        uint32_t startIndexLocation = 0;
        uint32_t startVertexIndex = 0;
        uint32_t startInstanceIndex = 0;
    };

    // Command List
    enum class QueueType
    {
        Graphics,
        Compute,
        Transfer,
        Present,

        Count
    };

    struct CommandListDesc
    {
        QueueType type = QueueType::Graphics;
    };

    class CommandList : public Resource
    {
    public:
        virtual ~CommandList() = default;
        virtual const CommandListDesc& GetDesc() const = 0;

        virtual void Open() = 0;
        virtual void Close() = 0;

        virtual void SetGraphicsState(const GraphicsState& state) = 0;
        virtual void SetComputeState(const ComputeState& state) = 0;

        virtual void Draw(const DrawArguments& args) = 0;
        virtual void DrawIndexed(const DrawArguments& args) = 0;
        virtual void DispatchCompute(uint32_t groupsX, uint32_t groupsY = 1, uint32_t groupsZ = 1) = 0;

        virtual void ClearState() = 0;
    };

    using CommandListHandle = Quest::RefCountPtr<CommandList>;
}
