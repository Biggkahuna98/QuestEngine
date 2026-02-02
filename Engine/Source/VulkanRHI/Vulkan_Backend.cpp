#include "Vulkan_Backend.h"

namespace qrhi::vulkan
{
    vk::Format ConvertFormat(Format format)
    {
        switch (format)
        {
        case Format::UNKNOWN: return vk::Format::eUndefined;
        case Format::R8_UINT: return vk::Format::eR8Uint;
        case Format::R8_SINT: return vk::Format::eR8Sint;
        case Format::R8_UNORM: return vk::Format::eR8Unorm;
        case Format::R8_SNORM: return vk::Format::eR8Snorm;
        case Format::RG8_UINT: return vk::Format::eR8G8Uint;
        case Format::RG8_SINT: return vk::Format::eR8G8Sint;
        case Format::RG8_UNORM: return vk::Format::eR8G8Unorm;
        case Format::RG8_SNORM: return vk::Format::eR8G8Snorm;
        case Format::R16_UINT: return vk::Format::eR16Uint;
        case Format::R16_SINT: return vk::Format::eR16Sint;
        case Format::R16_UNORM: return vk::Format::eR16Unorm;
        case Format::R16_SNORM: return vk::Format::eR16Snorm;
        case Format::R16_FLOAT: return vk::Format::eR16Sfloat;
        case Format::BGRA4_UNORM: return vk::Format::eB4G4R4A4UnormPack16;
        case Format::B5G6R5_UNORM: return vk::Format::eB5G6R5UnormPack16;
        case Format::B5G5R5A1_UNORM: return vk::Format::eB5G5R5A1UnormPack16;
        case Format::RGBA8_UINT: return vk::Format::eR8G8B8A8Uint;
        case Format::RGBA8_SINT: return vk::Format::eR8G8B8A8Sint;
        case Format::RGBA8_UNORM: return vk::Format::eR8G8B8A8Unorm;
        case Format::RGBA8_SNORM: return vk::Format::eR8G8B8A8Snorm;
        case Format::BGRA8_UNORM: return vk::Format::eB8G8R8A8Unorm;
        case Format::SRGBA8_UNORM: return vk::Format::eR8G8B8A8Srgb;
        case Format::SBGRA8_UNORM: return vk::Format::eB8G8R8A8Srgb;
        case Format::R10G10B10A2_UNORM: return vk::Format::eA2R10G10B10UnormPack32;
        case Format::R11G11B10_FLOAT: return vk::Format::eB10G11R11UfloatPack32;
        case Format::RG16_UINT: return vk::Format::eR16G16Uint;
        case Format::RG16_SINT: return vk::Format::eR16G16Sint;
        case Format::RG16_UNORM: return vk::Format::eR16G16Unorm;
        case Format::RG16_SNORM: return vk::Format::eR16G16Snorm;
        case Format::RG16_FLOAT: return vk::Format::eR16G16Sfloat;
        case Format::R32_UINT: return vk::Format::eR32Uint;
        case Format::R32_SINT: return vk::Format::eR32Sint;
        case Format::R32_FLOAT: return vk::Format::eR32Sfloat;
        case Format::RGBA16_UINT: return vk::Format::eR16G16B16A16Uint;
        case Format::RGBA16_SINT: return vk::Format::eR16G16B16A16Sint;
        case Format::RGBA16_FLOAT: return vk::Format::eR16G16B16A16Sfloat;
        case Format::RGBA16_UNORM: return vk::Format::eR16G16B16A16Unorm;
        case Format::RGBA16_SNORM: return vk::Format::eR16G16B16A16Snorm;
        case Format::RG32_UINT: return vk::Format::eR32G32Uint;
        case Format::RG32_SINT: return vk::Format::eR32G32Sint;
        case Format::RG32_FLOAT: return vk::Format::eR32G32Sfloat;
        case Format::RGB32_UINT: return vk::Format::eR32G32B32Uint;
        case Format::RGB32_SINT: return vk::Format::eR32G32B32Sint;
        case Format::RGB32_FLOAT: return vk::Format::eR32G32B32Sfloat;
        case Format::RGBA32_UINT: return vk::Format::eR32G32B32A32Uint;
        case Format::RGBA32_SINT: return vk::Format::eR32G32B32A32Sint;
        case Format::RGBA32_FLOAT: return vk::Format::eR32G32B32A32Sfloat;
        case Format::D16: return vk::Format::eD16Unorm;
        case Format::D24S8: return vk::Format::eD24UnormS8Uint;
        case Format::X24G8_UINT: return vk::Format::eD24UnormS8Uint;
        case Format::D32: return vk::Format::eD32Sfloat;
        case Format::D32S8: return vk::Format::eD32SfloatS8Uint;
        case Format::X32G8_UINT: return vk::Format::eD32SfloatS8Uint;
        case Format::BC1_UNORM: return vk::Format::eBc1RgbaUnormBlock;
        case Format::BC1_UNORM_SRGB: return vk::Format::eBc1RgbaSrgbBlock;
        case Format::BC2_UNORM: return vk::Format::eBc2UnormBlock;
        case Format::BC2_UNORM_SRGB: return vk::Format::eBc2SrgbBlock;
        case Format::BC3_UNORM: return vk::Format::eBc3UnormBlock;
        case Format::BC3_UNORM_SRGB: return vk::Format::eBc3SrgbBlock;
        case Format::BC4_UNORM: return vk::Format::eBc4UnormBlock;
        case Format::BC4_SNORM: return vk::Format::eBc4SnormBlock;
        case Format::BC5_UNORM: return vk::Format::eBc5UnormBlock;
        case Format::BC5_SNORM: return vk::Format::eBc5SnormBlock;
        case Format::BC6H_UFLOAT: return vk::Format::eBc6HUfloatBlock;
        case Format::BC6H_SFLOAT: return vk::Format::eBc6HSfloatBlock;
        case Format::BC7_UNORM: return vk::Format::eBc7UnormBlock;
        case Format::BC7_UNORM_SRGB: return vk::Format::eBc7SrgbBlock;
        default: return vk::Format::eUndefined;
        }
    }

    vk::SamplerAddressMode ConvertSamplerAddressMode(SamplerAddressMode mode)
    {
        switch (mode)
        {
        case SamplerAddressMode::Repeat:
            return vk::SamplerAddressMode::eRepeat;
        case SamplerAddressMode::MirrorRepeat:
            return vk::SamplerAddressMode::eMirroredRepeat;
        case SamplerAddressMode::ClampToEdge:
            return vk::SamplerAddressMode::eClampToEdge;
        case SamplerAddressMode::ClampToBorder:
            return vk::SamplerAddressMode::eClampToBorder;
        case SamplerAddressMode::MirrorClampToEdge:
            return vk::SamplerAddressMode::eMirrorClampToEdge;
        default:
            return vk::SamplerAddressMode::eClampToEdge;
        }
    }

    vk::PipelineStageFlagBits2 ConvertShaderTypeToPipelineStage(ShaderType type)
    {
        if (type == ShaderType::All)
            return vk::PipelineStageFlagBits2::eAllCommands;

        uint32_t result = 0;

        if ((type & ShaderType::Compute) != 0) result |= static_cast<uint32_t>(vk::PipelineStageFlagBits2::eComputeShader);
        if ((type & ShaderType::Vertex) != 0) result |= static_cast<uint32_t>(vk::PipelineStageFlagBits2::eVertexShader);
        if ((type & ShaderType::Fragment) != 0) result |= static_cast<uint32_t>(vk::PipelineStageFlagBits2::eFragmentShader);

        return static_cast<vk::PipelineStageFlagBits2>(result);

    }

    vk::ShaderStageFlagBits ConvertShaderType(ShaderType type)
    {
        if (type == ShaderType::All)
            return vk::ShaderStageFlagBits::eAll;

        if (type == ShaderType::AllGraphics)
            return vk::ShaderStageFlagBits::eAllGraphics;

        // Works because values match vulkan values
        return static_cast<vk::ShaderStageFlagBits>(type);
    }

    vk::PrimitiveTopology ConvertPrimitiveTopology(PrimitiveType type)
    {
        switch (type)
        {
            case PrimitiveType::PointList: return vk::PrimitiveTopology::ePointList;
            case PrimitiveType::LineList: return vk::PrimitiveTopology::eLineList;
            case PrimitiveType::LineStrip: return vk::PrimitiveTopology::eLineStrip;
            case PrimitiveType::TriangleList: return vk::PrimitiveTopology::eTriangleList;
            case PrimitiveType::TriangleStrip: return vk::PrimitiveTopology::eTriangleStrip;
            case PrimitiveType::TriangleFan: return vk::PrimitiveTopology::eTriangleFan;
            case PrimitiveType::TriangleListWithAdjacency: return vk::PrimitiveTopology::eTriangleListWithAdjacency;
            case PrimitiveType::TriangleStripWithAdjacency: return vk::PrimitiveTopology::eTriangleStripWithAdjacency;
            case PrimitiveType::PatchList: return vk::PrimitiveTopology::ePatchList;
            default: return vk::PrimitiveTopology::eTriangleList;
        }
    }

    vk::PolygonMode ConvertFillMode(RasterFillMode mode)
    {
        switch (mode)
        {
            case RasterFillMode::Solid: return vk::PolygonMode::eFill;
            case RasterFillMode::Wireframe: return vk::PolygonMode::eLine;
            default: return vk::PolygonMode::eFill;
        }
    }

    vk::CullModeFlagBits ConvertCullMode(RasterCullMode mode)
    {
        switch (mode)
        {
            case RasterCullMode::Back: return vk::CullModeFlagBits::eBack;
            case RasterCullMode::Front: return vk::CullModeFlagBits::eFront;
            case RasterCullMode::None: return vk::CullModeFlagBits::eNone;
            default: return vk::CullModeFlagBits::eNone;
        }
    }

    vk::CompareOp ConvertCompareOp(ComparisonFunc op)
    {
        switch (op)
        {
            case ComparisonFunc::Never: return vk::CompareOp::eNever;
            case ComparisonFunc::Less: return vk::CompareOp::eLess;
            case ComparisonFunc::Equal: return vk::CompareOp::eEqual;
            case ComparisonFunc::LessOrEqual: return vk::CompareOp::eLessOrEqual;
            case ComparisonFunc::Greater: return vk::CompareOp::eGreater;
            case ComparisonFunc::NotEqual: return vk::CompareOp::eNotEqual;
            case ComparisonFunc::GreaterOrEqual: return vk::CompareOp::eGreaterOrEqual;
            case ComparisonFunc::Always: return vk::CompareOp::eAlways;
            default: return vk::CompareOp::eAlways;
        }
    }

    vk::StencilOp ConvertStencilOp(StencilOp op)
    {
        switch (op)
        {
            case StencilOp::Keep: return vk::StencilOp::eKeep;
            case StencilOp::Zero: return vk::StencilOp::eZero;
            case StencilOp::Replace: return vk::StencilOp::eReplace;
            case StencilOp::IncrementAndClamp: return vk::StencilOp::eIncrementAndClamp;
            case StencilOp::DecrementAndClamp: return vk::StencilOp::eDecrementAndClamp;
            case StencilOp::Invert: return vk::StencilOp::eInvert;
            case StencilOp::IncrementAndWrap: return vk::StencilOp::eIncrementAndWrap;
            case StencilOp::DecrementAndWrap: return vk::StencilOp::eDecrementAndWrap;
            default: return vk::StencilOp::eKeep;
        }
    }

    vk::StencilOpState ConvertStencilState(const DepthStencilState& depthStencilState,
        const DepthStencilState::StencilOpDesc& desc)
    {
        return vk::StencilOpState()
                .setFailOp(ConvertStencilOp(desc.failOp))
                .setPassOp(ConvertStencilOp(desc.passOp))
                .setDepthFailOp(ConvertStencilOp(desc.depthFailOp))
                .setCompareOp(ConvertCompareOp(desc.stencilFunc))
                .setCompareMask(depthStencilState.stencilReadMask)
                .setWriteMask(depthStencilState.stencilWriteMask)
                .setReference(depthStencilState.stencilRefValue);
    }

    vk::BlendFactor ConvertBlendValue(BlendFactor value)
    {
        switch (value)
        {
            case BlendFactor::Zero: return vk::BlendFactor::eZero;
            case BlendFactor::One: return vk::BlendFactor::eOne;
            case BlendFactor::SrcColor: return vk::BlendFactor::eSrcColor;
            case BlendFactor::OneMinusSrcColor: return vk::BlendFactor::eOneMinusSrcColor;
            case BlendFactor::SrcAlpha: return vk::BlendFactor::eSrcAlpha;
            case BlendFactor::OneMinusSrcAlpha: return vk::BlendFactor::eOneMinusSrcAlpha;
            case BlendFactor::DstAlpha: return vk::BlendFactor::eDstAlpha;
            case BlendFactor::OneMinusDstAlpha: return vk::BlendFactor::eOneMinusDstAlpha;
            case BlendFactor::DstColor: return vk::BlendFactor::eDstColor;
            case BlendFactor::OneMinusDstColor: return vk::BlendFactor::eOneMinusDstColor;
            case BlendFactor::SrcAlphaSaturate: return vk::BlendFactor::eSrcAlphaSaturate;
            case BlendFactor::ConstantColor: return vk::BlendFactor::eConstantColor;
            case BlendFactor::OneMinusConstantColor: return vk::BlendFactor::eOneMinusConstantColor;
            case BlendFactor::Src1Color: return vk::BlendFactor::eSrc1Color;
            case BlendFactor::OneMinusSrc1Color: return vk::BlendFactor::eOneMinusSrc1Color;
            case BlendFactor::Src1Alpha: return vk::BlendFactor::eSrc1Alpha;
            case BlendFactor::OneMinusSrc1Alpha: return vk::BlendFactor::eOneMinusSrc1Alpha;
            default: return vk::BlendFactor::eZero;
        }
    }

    vk::BlendOp ConvertBlendOp(BlendOp value)
    {
        switch (value)
        {
            case BlendOp::Add: return vk::BlendOp::eAdd;
            case BlendOp::Subtract: return vk::BlendOp::eSubtract;
            case BlendOp::ReverseSubtract: return vk::BlendOp::eReverseSubtract;
            case BlendOp::Min: return vk::BlendOp::eMin;
            case BlendOp::Max: return vk::BlendOp::eMax;
            default: return vk::BlendOp::eAdd;
        }
    }

    vk::ColorComponentFlags ConvertColorMask(ColorMask mask)
    {
        return vk::ColorComponentFlags(static_cast<uint8_t>(mask));
    }

    vk::PipelineColorBlendAttachmentState ConvertBlendState(const RenderTarget& renderTarget)
    {
        return vk::PipelineColorBlendAttachmentState()
                .setBlendEnable(renderTarget.blendEnable)
                .setSrcColorBlendFactor(ConvertBlendValue(renderTarget.srcBlend))
                .setDstColorBlendFactor(ConvertBlendValue(renderTarget.dstBlend))
                .setColorBlendOp(ConvertBlendOp(renderTarget.blendOp))
                .setSrcAlphaBlendFactor(ConvertBlendValue(renderTarget.srcBlendAlpha))
                .setDstAlphaBlendFactor(ConvertBlendValue(renderTarget.dstBlendAlpha))
                .setAlphaBlendOp(ConvertBlendOp(renderTarget.blendOpAlpha))
                .setColorWriteMask(ConvertColorMask(renderTarget.colorWriteMask));
    }

    vk::DescriptorType ConvertResourceType(BindingResourceType type)
    {
        switch (type)
        {
            case BindingResourceType::Texture_SRV: return vk::DescriptorType::eSampledImage;
            case BindingResourceType::Texture_UAV: return vk::DescriptorType::eStorageImage;
            case BindingResourceType::TypedBuffer_SRV: return vk::DescriptorType::eUniformTexelBuffer;
            case BindingResourceType::TypedBuffer_UAV: return vk::DescriptorType::eStorageTexelBuffer;
            case BindingResourceType::StructuredBuffer_SRV: return vk::DescriptorType::eStorageBuffer;
            case BindingResourceType::StructuredBuffer_UAV: return vk::DescriptorType::eStorageBuffer;
            case BindingResourceType::RawBuffer_SRV: return vk::DescriptorType::eStorageBuffer;
            case BindingResourceType::RawBuffer_UAV: return vk::DescriptorType::eStorageBuffer;
            case BindingResourceType::ConstantBuffer: return vk::DescriptorType::eUniformBuffer;
            case BindingResourceType::VolatileConstantBuffer: return vk::DescriptorType::eUniformBufferDynamic;
            case BindingResourceType::Sampler: return vk::DescriptorType::eSampler;
            case BindingResourceType::PushConstants: return vk::DescriptorType::eUniformBuffer; // not really
        default:
            QE_ASSERT(false);
        }
    }

    void VulkanContext::nameVKObject(const void* handle, const vk::ObjectType objType, const char* name) const
    {
        if (!(name && *name && handle))
            return;

        auto info = vk::DebugUtilsObjectNameInfoEXT()
            .setObjectType(objType)
            .setObjectHandle(reinterpret_cast<uint64_t>(handle))
            .setPObjectName(name);
        device.setDebugUtilsObjectNameEXT(info);
    }

    void VulkanContext::info(std::string_view message)
    {
        messageCallback->Message(MessageSeverity::Info, message);
    }

    void VulkanContext::warning(std::string_view message)
    {
        messageCallback->Message(MessageSeverity::Warning, message);
    }

    void VulkanContext::error(std::string_view message)
    {
        messageCallback->Message(MessageSeverity::Error, message);
    }
}
