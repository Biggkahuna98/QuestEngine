#pragma once

#include <string>
#include <string_view>

#include <Core/OpaqueObject.h>
#include <Core/RefCounting.h>

#include "glm/glm.hpp"

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

namespace Quest
{
    // Constants
    static constexpr uint32_t c_MaxBindlessSlots = 16;
    static constexpr uint32_t c_MaxBindingLayouts = 8;
    static constexpr uint32_t c_MaxDynamicUniformBuffersPerLayout = 6;

    enum class GraphicsAPI
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

    class Resource : public RefCounted
    {
    public:
        // The API specific implementation, i.e. VulkanBuffer which gives access to the underlying types
        virtual OpaqueObject GetNativeType() = 0;
        virtual void SetDebugName(const std::string& name) = 0;
    };

    using ResourceHandle = RefCountPtr<Resource>;
}