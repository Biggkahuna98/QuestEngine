#include "VulkanCommon.h"

namespace qrhi::vulkan
{
    vk::Format ConvertFormat(Format format)
    {
        switch (format)
        {
            // Single/Dual Channel
            case Format::R8_UNORM:              return vk::Format::eR8Unorm;
            case Format::R8_SNORM:              return vk::Format::eR8Snorm;
            case Format::RG8_UNORM:             return vk::Format::eR8G8Unorm;
            case Format::RG8_SNORM:             return vk::Format::eR8G8Snorm;
            case Format::R16_FLOAT:             return vk::Format::eR16Sfloat;
            case Format::RG16_FLOAT:            return vk::Format::eR16G16Sfloat;
            case Format::RG16_UNORM:            return vk::Format::eR16G16Unorm;
            case Format::R32_UINT:              return vk::Format::eR32Uint;
            case Format::R32_SINT:              return vk::Format::eR32Sint;
            case Format::R32_FLOAT:             return vk::Format::eR32Sfloat;
            case Format::RG32_FLOAT:            return vk::Format::eR32G32Sfloat;

            // Color Render Targets & Textures
            case Format::RGBA8_UNORM:           return vk::Format::eR8G8B8A8Unorm;
            case Format::RGBA8_SNORM:           return vk::Format::eR8G8B8A8Snorm;
            case Format::SRGBA8_UNORM:          return vk::Format::eR8G8B8A8Srgb;
            case Format::BGRA8_UNORM:           return vk::Format::eB8G8R8A8Unorm;
            case Format::SBGRA8_UNORM:          return vk::Format::eB8G8R8A8Srgb;
            case Format::R10G10B10A2_UNORM:     return vk::Format::eA2R10G10B10UnormPack32;
            case Format::R11G11B10_FLOAT:       return vk::Format::eB10G11R11UfloatPack32;
            case Format::RGBA16_FLOAT:          return vk::Format::eR16G16B16A16Sfloat;
            case Format::RGBA16_UNORM:          return vk::Format::eR16G16B16A16Unorm;
            case Format::RGBA32_FLOAT:          return vk::Format::eR32G32B32A32Sfloat;

            // Integer / Compute
            case Format::R8_UINT:               return vk::Format::eR8Uint;
            case Format::R8_SINT:               return vk::Format::eR8Sint;
            case Format::RG8_UINT:              return vk::Format::eR8G8Uint;
            case Format::R16_UINT:              return vk::Format::eR16Uint;
            case Format::R16_SINT:              return vk::Format::eR16Sint;
            case Format::RG16_UINT:             return vk::Format::eR16G16Uint;
            case Format::RG16_SINT:             return vk::Format::eR16G16Sint;
            case Format::RGBA8_UINT:            return vk::Format::eR8G8B8A8Uint;
            case Format::RGBA8_SINT:            return vk::Format::eR8G8B8A8Sint;
            case Format::RGBA16_UINT:           return vk::Format::eR16G16B16A16Uint;
            case Format::RGBA16_SINT:           return vk::Format::eR16G16B16A16Sint;
            case Format::RGBA32_UINT:           return vk::Format::eR32G32B32A32Uint;
            case Format::RGBA32_SINT:           return vk::Format::eR32G32B32A32Sint;
            case Format::RG32_UINT:             return vk::Format::eR32G32Uint;
            case Format::RG32_SINT:             return vk::Format::eR32G32Sint;

            // Depth / Stencil
            case Format::D16:                   return vk::Format::eD16Unorm;
            case Format::D24S8:                 return vk::Format::eD24UnormS8Uint;
            case Format::D32:                   return vk::Format::eD32Sfloat;
            case Format::D32S8:                 return vk::Format::eD32SfloatS8Uint;
            case Format::X24G8_UINT:            return vk::Format::eD24UnormS8Uint;
            case Format::X32G8_UINT:            return vk::Format::eD32SfloatS8Uint;

            // Block Compression
            case Format::BC1_UNORM:             return vk::Format::eBc1RgbaUnormBlock;
            case Format::BC1_UNORM_SRGB:        return vk::Format::eBc1RgbaSrgbBlock;
            case Format::BC3_UNORM:             return vk::Format::eBc3UnormBlock;
            case Format::BC3_UNORM_SRGB:        return vk::Format::eBc3SrgbBlock;
            case Format::BC4_UNORM:             return vk::Format::eBc4UnormBlock;
            case Format::BC4_SNORM:             return vk::Format::eBc4SnormBlock;
            case Format::BC5_UNORM:             return vk::Format::eBc5UnormBlock;
            case Format::BC5_SNORM:             return vk::Format::eBc5SnormBlock;
            case Format::BC6H_UFLOAT:           return vk::Format::eBc6HUfloatBlock;
            case Format::BC6H_SFLOAT:           return vk::Format::eBc6HSfloatBlock;
            case Format::BC7_UNORM:             return vk::Format::eBc7UnormBlock;
            case Format::BC7_UNORM_SRGB:        return vk::Format::eBc7SrgbBlock;

            case Format::UNKNOWN:
            default:                            return vk::Format::eUndefined;
        }
    }
    vk::DescriptorType ConvertDescriptorType(BindingResourceType type)
    {
        switch (type)
        {
            case BindingResourceType::Texture_SRV:           return vk::DescriptorType::eSampledImage;
            case BindingResourceType::Texture_UAV:           return vk::DescriptorType::eStorageImage;
            case BindingResourceType::StorageBuffer_SRV:     return vk::DescriptorType::eStorageBuffer;
            case BindingResourceType::StorageBuffer_UAV:     return vk::DescriptorType::eStorageBuffer;
            case BindingResourceType::UniformBuffer:         return vk::DescriptorType::eUniformBuffer;
            case BindingResourceType::DynamicUniformBuffer:  return vk::DescriptorType::eUniformBufferDynamic;
            case BindingResourceType::Sampler:               return vk::DescriptorType::eSampler;
            case BindingResourceType::PushConstants:         return vk::DescriptorType::eUniformBuffer;

            case BindingResourceType::None:
            default:                                         return vk::DescriptorType::eSampler; // no undefined equivalent
        }
    }

    vk::PrimitiveTopology ConvertPrimitiveType(PrimitiveType type)
    {
        switch (type)
        {
            case PrimitiveType::PointList:                    return vk::PrimitiveTopology::ePointList;
            case PrimitiveType::LineList:                     return vk::PrimitiveTopology::eLineList;
            case PrimitiveType::LineStrip:                    return vk::PrimitiveTopology::eLineStrip;
            case PrimitiveType::TriangleList:                 return vk::PrimitiveTopology::eTriangleList;
            case PrimitiveType::TriangleStrip:                return vk::PrimitiveTopology::eTriangleStrip;
            case PrimitiveType::TriangleFan:                  return vk::PrimitiveTopology::eTriangleFan;
            case PrimitiveType::TriangleListWithAdjacency:    return vk::PrimitiveTopology::eTriangleListWithAdjacency;
            case PrimitiveType::TriangleStripWithAdjacency:   return vk::PrimitiveTopology::eTriangleStripWithAdjacency;
            case PrimitiveType::PatchList:                    return vk::PrimitiveTopology::ePatchList;

            default:
                return vk::PrimitiveTopology::eTriangleList;
        }
    }

    vk::ShaderStageFlagBits ConvertShaderType(ShaderType type)
    {
        switch (type)
        {
            case ShaderType::Vertex:                        return vk::ShaderStageFlagBits::eVertex;
            case ShaderType::Fragment:                      return vk::ShaderStageFlagBits::eFragment;
            case ShaderType::Compute:                       return vk::ShaderStageFlagBits::eCompute;
            case ShaderType::AllGraphics:                   return vk::ShaderStageFlagBits::eAllGraphics;
            case ShaderType::All:                           return vk::ShaderStageFlagBits::eAll;
            default:                                        return vk::ShaderStageFlagBits::eAll;
        }
    }

    vk::WriteDescriptorSet GenerateWriteDescriptorData(uint32_t bindingLocation, uint32_t arrayElement,
        vk::DescriptorType descriptorType, vk::DescriptorSet *dstSet, vk::DescriptorImageInfo *imageInfo,
        vk::DescriptorBufferInfo *bufferInfo, vk::BufferView *bufferView, const void* pNext)
    {
        vk::WriteDescriptorSet descriptorSet = vk::WriteDescriptorSet()
            .setDstSet(*dstSet)
            .setDstBinding(bindingLocation)
            .setDstArrayElement(arrayElement)
            .setDescriptorCount(1)
            .setDescriptorType(descriptorType)
            .setPImageInfo(imageInfo)
            .setPBufferInfo(bufferInfo)
            .setPTexelBufferView(bufferView)
            .setPNext(pNext);
        return descriptorSet;
    }
}