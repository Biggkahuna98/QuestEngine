#pragma once

#include <cmath>
#include <vector>
#include "Utility/RefCounting.h"

namespace qrhi
{
    // Constants
    static constexpr uint32_t c_MaxRenderTargets = 8;
    static constexpr uint32_t c_MaxViewports = 8;
    static constexpr uint32_t c_MaxVertexAttributes = 16;
    static constexpr uint32_t c_MaxBindingLayouts = 8;
    static constexpr uint32_t c_MaxPushConstantSize = 128;
    static constexpr uint32_t c_MaxBindlessSlots = 16;

    // RHI resources should derive from this
    class Resource : public Quest::IRefCountable
    {
    protected:
        ~Resource() override = default;
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

    struct Viewport
    {
        float minX, maxX;
        float minY, maxY;
        float minZ, maxZ;

        Viewport() : minX(0.f), maxX(0.f), minY(0.f), maxY(0.f), minZ(0.f), maxZ(1.f) { }

        Viewport(float width, float height) : minX(0.f), maxX(width), minY(0.f), maxY(height), minZ(0.f), maxZ(1.f) { }

        Viewport(float _minX, float _maxX, float _minY, float _maxY, float _minZ, float _maxZ)
            : minX(_minX), maxX(_maxX), minY(_minY), maxY(_maxY), minZ(_minZ), maxZ(_maxZ)
        { }

        bool operator ==(const Viewport& b) const
        {
            return minX == b.minX
                && minY == b.minY
                && minZ == b.minZ
                && maxX == b.maxX
                && maxY == b.maxY
                && maxZ == b.maxZ;
        }
        bool operator !=(const Viewport& b) const { return !(*this == b); }

        [[nodiscard]] float width() const { return maxX - minX; }
        [[nodiscard]] float height() const { return maxY - minY; }
    };

    struct Rect
    {
        int minX, maxX;
        int minY, maxY;

        Rect() : minX(0), maxX(0), minY(0), maxY(0) { }
        Rect(int width, int height) : minX(0), maxX(width), minY(0), maxY(height) { }
        Rect(int _minX, int _maxX, int _minY, int _maxY) : minX(_minX), maxX(_maxX), minY(_minY), maxY(_maxY) { }
        explicit Rect(const Viewport& viewport)
            : minX(int(floorf(viewport.minX)))
            , maxX(int(ceilf(viewport.maxX)))
            , minY(int(floorf(viewport.minY)))
            , maxY(int(ceilf(viewport.maxY)))
        {
        }

        bool operator ==(const Rect& b) const {
            return minX == b.minX && minY == b.minY && maxX == b.maxX && maxY == b.maxY;
        }
        bool operator !=(const Rect& b) const { return !(*this == b); }

        [[nodiscard]] int width() const { return maxX - minX; }
        [[nodiscard]] int height() const { return maxY - minY; }
    };

    enum class GraphicsAPI
    {
        Vulkan
    };

    enum class NativeObjectType
    {
        VK_Device,
        VK_PhysicalDevice,
        VK_Instance,
        VK_Queue,
        VK_CommandBuffer,
        VK_Buffer,
        VK_Image,
        VK_ImageView,
        VK_Sampler,
        VK_ShaderModule,
        VK_RenderPass,
        VK_Framebuffer,
        VK_DescriptorPool,
        VK_DescriptorSetLayout,
        VK_DescriptorSet,
        VK_PipelineLayout,
        VK_Pipeline
    };

    enum class Format : uint8_t
    {
        UNKNOWN,

        R8_UINT,
        R8_SINT,
        R8_UNORM,
        R8_SNORM,
        RG8_UINT,
        RG8_SINT,
        RG8_UNORM,
        RG8_SNORM,
        R16_UINT,
        R16_SINT,
        R16_UNORM,
        R16_SNORM,
        R16_FLOAT,
        BGRA4_UNORM,
        B5G6R5_UNORM,
        B5G5R5A1_UNORM,
        RGBA8_UINT,
        RGBA8_SINT,
        RGBA8_UNORM,
        RGBA8_SNORM,
        BGRA8_UNORM,
        SRGBA8_UNORM,
        SBGRA8_UNORM,
        R10G10B10A2_UNORM,
        R11G11B10_FLOAT,
        RG16_UINT,
        RG16_SINT,
        RG16_UNORM,
        RG16_SNORM,
        RG16_FLOAT,
        R32_UINT,
        R32_SINT,
        R32_FLOAT,
        RGBA16_UINT,
        RGBA16_SINT,
        RGBA16_FLOAT,
        RGBA16_UNORM,
        RGBA16_SNORM,
        RG32_UINT,
        RG32_SINT,
        RG32_FLOAT,
        RGB32_UINT,
        RGB32_SINT,
        RGB32_FLOAT,
        RGBA32_UINT,
        RGBA32_SINT,
        RGBA32_FLOAT,

        COUNT
    };

    enum class FormatUsedWith : uint8_t
    {
        Integer,
        Normalized,
        Float,
        DepthStencil
    };

    enum class CPUAccessMode : uint8_t
    {
        None,
        Read,
        Write
    };

    using GpuVirtualAddress = uint64_t;
    struct GpuVirtualAddressAndStride
    {
        GpuVirtualAddress address;
        uint64_t stride;
    };

    // Raster state
    enum class RasterFillMode : uint8_t
    {
        Solid,
        Wireframe
    };

    enum class RasterCullMode : uint8_t
    {
        Back,
        Front,
        None
    };

    struct RasterState
    {
        RasterFillMode fillMode = RasterFillMode::Solid;
        RasterCullMode cullMode = RasterCullMode::Back;
        bool frontCounterClockwise = false;
        bool depthClipEnable = false;
        bool scissorEnable = false;
        bool multisampleEnable = false;
        bool antialiasedLineEnable = false;
        int depthBias = 0;
        float depthBiasClamp = 0.f;
        float slopeScaledDepthBias = 0.f;

        bool quadFillEnable = false;

        constexpr RasterState& setFillMode(RasterFillMode value) { fillMode = value; return *this; }
        constexpr RasterState& setFillSolid() { fillMode = RasterFillMode::Solid; return *this; }
        constexpr RasterState& setFillWireframe() { fillMode = RasterFillMode::Wireframe; return *this; }
        constexpr RasterState& setCullMode(RasterCullMode value) { cullMode = value; return *this; }
        constexpr RasterState& setCullBack() { cullMode = RasterCullMode::Back; return *this; }
        constexpr RasterState& setCullFront() { cullMode = RasterCullMode::Front; return *this; }
        constexpr RasterState& setCullNone() { cullMode = RasterCullMode::None; return *this; }
        constexpr RasterState& setFrontCounterClockwise(bool value) { frontCounterClockwise = value; return *this; }
        constexpr RasterState& setDepthClipEnable(bool value) { depthClipEnable = value; return *this; }
        constexpr RasterState& enableDepthClip() { depthClipEnable = true; return *this; }
        constexpr RasterState& disableDepthClip() { depthClipEnable = false; return *this; }
        constexpr RasterState& setScissorEnable(bool value) { scissorEnable = value; return *this; }
        constexpr RasterState& enableScissor() { scissorEnable = true; return *this; }
        constexpr RasterState& disableScissor() { scissorEnable = false; return *this; }
        constexpr RasterState& setMultisampleEnable(bool value) { multisampleEnable = value; return *this; }
        constexpr RasterState& enableMultisample() { multisampleEnable = true; return *this; }
        constexpr RasterState& disableMultisample() { multisampleEnable = false; return *this; }
        constexpr RasterState& setAntialiasedLineEnable(bool value) { antialiasedLineEnable = value; return *this; }
        constexpr RasterState& enableAntialiasedLine() { antialiasedLineEnable = true; return *this; }
        constexpr RasterState& disableAntialiasedLine() { antialiasedLineEnable = false; return *this; }
        constexpr RasterState& setDepthBias(int value) { depthBias = value; return *this; }
        constexpr RasterState& setDepthBiasClamp(float value) { depthBiasClamp = value; return *this; }
        constexpr RasterState& setSlopeScaleDepthBias(float value) { slopeScaledDepthBias = value; return *this; }
        constexpr RasterState& setQuadFillEnable(bool value) { quadFillEnable = value; return *this; }
        constexpr RasterState& enableQuadFill() { quadFillEnable = true; return *this; }
        constexpr RasterState& disableQuadFill() { quadFillEnable = false; return *this; }
    };

    // Depth/stencil state
    enum class StencilOp : uint8_t
    {
        Keep = 1,
        Zero = 2,
        Replace = 3,
        IncrementAndClamp = 4,
        DecrementAndClamp = 5,
        Invert = 6,
        IncrementAndWrap = 7,
        DecrementAndWrap = 8
    };

    enum class ComparisonFunc : uint8_t
    {
        Never = 1,
        Less = 2,
        Equal = 3,
        LessOrEqual = 4,
        Greater = 5,
        NotEqual = 6,
        GreaterOrEqual = 7,
        Always = 8
    };

    struct DepthStencilState
    {
        struct StencilOpDesc
        {
            StencilOp failOp = StencilOp::Keep;
            StencilOp depthFailOp = StencilOp::Keep;
            StencilOp passOp = StencilOp::Keep;
            ComparisonFunc stencilFunc = ComparisonFunc::Always;

            constexpr StencilOpDesc& setFailOp(StencilOp value) { failOp = value; return *this; }
            constexpr StencilOpDesc& setDepthFailOp(StencilOp value) { depthFailOp = value; return *this; }
            constexpr StencilOpDesc& setPassOp(StencilOp value) { passOp = value; return *this; }
            constexpr StencilOpDesc& setStencilFunc(ComparisonFunc value) { stencilFunc = value; return *this; }
        };

        bool depthTestEnable = true;
        bool depthWriteEnable = true;
        ComparisonFunc depthFunc = ComparisonFunc::Less;
        bool stencilEnable = false;
        uint8_t stencilReadMask = 0xff;
        uint8_t stencilWriteMask = 0xff;
        uint8_t stencilRefValue = 0;
        bool dynamicStencilRef = false;
        StencilOpDesc frontFaceStencil;
        StencilOpDesc backFaceStencil;

        constexpr DepthStencilState& setDepthTestEnable(bool value) { depthTestEnable = value; return *this; }
        constexpr DepthStencilState& enableDepthTest() { depthTestEnable = true; return *this; }
        constexpr DepthStencilState& disableDepthTest() { depthTestEnable = false; return *this; }
        constexpr DepthStencilState& setDepthWriteEnable(bool value) { depthWriteEnable = value; return *this; }
        constexpr DepthStencilState& enableDepthWrite() { depthWriteEnable = true; return *this; }
        constexpr DepthStencilState& disableDepthWrite() { depthWriteEnable = false; return *this; }
        constexpr DepthStencilState& setDepthFunc(ComparisonFunc value) { depthFunc = value; return *this; }
        constexpr DepthStencilState& setStencilEnable(bool value) { stencilEnable = value; return *this; }
        constexpr DepthStencilState& enableStencil() { stencilEnable = true; return *this; }
        constexpr DepthStencilState& disableStencil() { stencilEnable = false; return *this; }
        constexpr DepthStencilState& setStencilReadMask(uint8_t value) { stencilReadMask = value; return *this; }
        constexpr DepthStencilState& setStencilWriteMask(uint8_t value) { stencilWriteMask = value; return *this; }
        constexpr DepthStencilState& setStencilRefValue(uint8_t value) { stencilRefValue = value; return *this; }
        constexpr DepthStencilState& setFrontFaceStencil(const StencilOpDesc& value) { frontFaceStencil = value; return *this; }
        constexpr DepthStencilState& setBackFaceStencil(const StencilOpDesc& value) { backFaceStencil = value; return *this; }
        constexpr DepthStencilState& setDynamicStencilRef(bool value) { dynamicStencilRef = value; return *this; }
    };

    // Viewport state
    struct ViewportState
    {
        // leaving these vectors empty means no state is set
        // TODO: refactor to be a fixed capacity array later
        std::vector<Viewport> viewports;
        std::vector<Rect> scissorRects;

        ViewportState& addViewport(const Viewport& v) { viewports.push_back(v); return *this; }
        ViewportState& addScissorRect(const Rect& r) { scissorRects.push_back(r); return *this; }
        ViewportState& addViewportAndScissorRect(const Viewport& v) { return addViewport(v).addScissorRect(Rect(v)); }
    };

    enum class BlendFactor : uint8_t
    {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        DstColor,
        OneMinusDstColor,
        SrcAlphaSaturate,
        ConstantColor,
        OneMinusConstantColor,
        Src1Color,
        OneMinusSrc1Color,
        Src1Alpha,
        OneMinusSrc1Alpha
    };

    enum class BlendOp : uint8_t
    {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    enum class ColorMask : uint8_t
    {
        Red = 1,
        Green = 2,
        Blue = 4,
        Alpha = 8,
        All = 0xF
    };

    struct RenderTarget
    {
        bool blendEnable = false;
        BlendFactor srcBlend = BlendFactor::One;
        BlendFactor dstBlend = BlendFactor::Zero;
        BlendOp blendOp = BlendOp::Add;
        BlendFactor srcBlendAlpha = BlendFactor::One;
        BlendFactor dstBlendAlpha = BlendFactor::Zero;
        BlendOp blendOpAlpha = BlendOp::Add;
        ColorMask colorWriteMask = ColorMask::All;

        constexpr RenderTarget& setBlendEnable(bool enable) { blendEnable = enable; return *this; }
        constexpr RenderTarget& enableBlend() { blendEnable = true; return *this; }
        constexpr RenderTarget& disableBlend() { blendEnable = false; return *this; }
        constexpr RenderTarget& setSrcBlend(BlendFactor value) { srcBlend = value; return *this; }
        constexpr RenderTarget& setDestBlend(BlendFactor value) { dstBlend = value; return *this; }
        constexpr RenderTarget& setBlendOp(BlendOp value) { blendOp = value; return *this; }
        constexpr RenderTarget& setSrcBlendAlpha(BlendFactor value) { srcBlendAlpha = value; return *this; }
        constexpr RenderTarget& setDestBlendAlpha(BlendFactor value) { dstBlendAlpha = value; return *this; }
        constexpr RenderTarget& setBlendOpAlpha(BlendOp value) { blendOpAlpha = value; return *this; }
        constexpr RenderTarget& setColorWriteMask(ColorMask value) { colorWriteMask = value; return *this; }

        constexpr bool operator ==(const RenderTarget& other) const
        {
            return blendEnable == other.blendEnable
                && srcBlend == other.srcBlend
                && dstBlend == other.dstBlend
                && blendOp == other.blendOp
                && srcBlendAlpha == other.srcBlendAlpha
                && dstBlendAlpha == other.dstBlendAlpha
                && blendOpAlpha == other.blendOpAlpha
                && colorWriteMask == other.colorWriteMask;
        }

        constexpr bool operator !=(const RenderTarget& other) const
        {
            return !(*this == other);
        }
    };

    struct BlendState
    {
        // TODO: CHANGE THIS MAGIC NUMBER
        RenderTarget renderTargets[c_MaxRenderTargets];
        bool alphaToCoverageEnable = false;

        constexpr BlendState& setRenderTarget( uint32_t index, const RenderTarget& target) { renderTargets[index] = target; return *this; }
        constexpr BlendState& setAlphaToCoverageEnable(bool enable) { alphaToCoverageEnable = enable; return *this; }
        constexpr BlendState& enableAlphaToCoverage() { alphaToCoverageEnable = true; return *this; }
        constexpr BlendState& disableAlphaToCoverage() { alphaToCoverageEnable = false; return *this; }

        constexpr bool operator ==(const BlendState& other) const
        {
            if (alphaToCoverageEnable != other.alphaToCoverageEnable)
                return false;

            for (uint32_t i = 0; i < c_MaxRenderTargets; ++i) // 8 = max rendertargets
            {
                if (renderTargets[i] != other.renderTargets[i])
                    return false;
            }

            return true;
        }

        constexpr bool operator !=(const BlendState& other) const
        {
            return !(*this == other);
        }
    };

    // Yoinked from NVRHI
    template <class T>
    void hash_combine(size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
}

namespace std
{
    template<> struct hash<qrhi::RenderTarget>
    {
        std::size_t operator()(qrhi::RenderTarget const& s) const noexcept
        {
            size_t hash = 0;
            qrhi::hash_combine(hash, s.blendEnable);
            qrhi::hash_combine(hash, s.srcBlend);
            qrhi::hash_combine(hash, s.dstBlend);
            qrhi::hash_combine(hash, s.blendOp);
            qrhi::hash_combine(hash, s.srcBlendAlpha);
            qrhi::hash_combine(hash, s.dstBlendAlpha);
            qrhi::hash_combine(hash, s.blendOpAlpha);
            qrhi::hash_combine(hash, s.colorWriteMask);
            return hash;
        }
    };

    template<> struct hash<qrhi::BlendState>
    {
        std::size_t operator()(qrhi::BlendState const& s) const noexcept
        {
            size_t hash = 0;
            qrhi::hash_combine(hash, s.alphaToCoverageEnable);
            for (const auto& target : s.renderTargets)
                qrhi::hash_combine(hash, target);
            return hash;
        }
    };
}
