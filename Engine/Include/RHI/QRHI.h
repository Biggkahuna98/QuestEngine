#pragma once

#include <string>
#include <string_view>

#include "Core/Core.h"
#include "Utility/OpaqueObject.h"
#include "Utility/RefCounting.h"

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
    enum class API
    {
        None, // no-op, doesn't do anything
        Vulkan // default
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

    class Resource : public Quest::RefCounted
    {
    public:
        virtual ~Resource() = default;

        // The API specific implementation, i.e. VulkanBuffer which gives access to the underlying types
        virtual Quest::OpaqueObject GetNativeType() = 0;
        virtual void SetDebugName(const std::string& name) = 0;
    };

    // Buffer
    struct BufferDesc
    {

    };

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
    enum class ShaderType
    {
        Vertex = BIT(0),
        Fragment = BIT(1),
        Compute = BIT(2),

        CombinedTypes // use the mask for which types are included
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
        std::string entry_point = "main"; // if slang, both main functions must have same name
        ShaderType type = ShaderType::Vertex;
        ShaderLanguage language = ShaderLanguage::Slang;

        // an idea to try to have multiple shaders in 1 file for slang
        uint32_t shaderTypeMask = 0;
    };

    class Shader : public Resource
    {
    public:
        virtual ~Shader() = default;
        virtual const ShaderDesc& GetDesc() const = 0;
    };

    using ShaderHandle = Quest::RefCountPtr<Shader>;

    // Pipeline
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

    struct GraphicsPipelineDesc
    {
        ShaderHandle vertexShader;
        ShaderHandle fragmentShader;

        FillMode fillMode = FillMode::Solid;
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
