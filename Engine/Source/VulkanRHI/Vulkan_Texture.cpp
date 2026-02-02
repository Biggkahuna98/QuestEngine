#include "Vulkan_Texture.h"

namespace qrhi::vulkan
{
    vk::ImageType ConvertTextureDimensionToImageType(TextureDimension dimension)
    {
        switch (dimension)
        {
            case TextureDimension::Texture1D:
            case TextureDimension::Texture1DArray:
                return vk::ImageType::e1D;

            case TextureDimension::Texture2D:
            case TextureDimension::Texture2DArray:
            case TextureDimension::TextureCube:
            case TextureDimension::TextureCubeArray:
            case TextureDimension::Texture2DMS:
            case TextureDimension::Texture2DMSArray:
                return vk::ImageType::e2D;

            case TextureDimension::Texture3D:
                return vk::ImageType::e3D;

            case TextureDimension::Unknown:
            default:
                return vk::ImageType::e2D;
        }
    }

    vk::ImageViewType ConvertTextureDimensionToImageViewType(TextureDimension dimension)
    {
        switch (dimension)
        {
            case TextureDimension::Texture1D:
                return vk::ImageViewType::e1D;

            case TextureDimension::Texture1DArray:
                return vk::ImageViewType::e1DArray;

            case TextureDimension::Texture2D:
            case TextureDimension::Texture2DMS:
                return vk::ImageViewType::e2D;

            case TextureDimension::Texture2DArray:
            case TextureDimension::Texture2DMSArray:
                return vk::ImageViewType::e2DArray;

            case TextureDimension::TextureCube:
                return vk::ImageViewType::eCube;

            case TextureDimension::TextureCubeArray:
                return vk::ImageViewType::eCubeArray;

            case TextureDimension::Texture3D:
                return vk::ImageViewType::e3D;

            case TextureDimension::Unknown:
            default:
                return vk::ImageViewType::e2D;
        }
    }

    vk::Extent3D PickImageExtent(const TextureDesc& desc)
    {
        return vk::Extent3D(desc.width, desc.height, desc.depth);
    }

    uint32_t PickImageLayers(const TextureDesc& desc)
    {
        return desc.arraySize;
    }

    vk::ImageUsageFlags PickImageUsageFlags(const TextureDesc& desc)
    {
        vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc;

        if (desc.isShaderResource) usage |= vk::ImageUsageFlagBits::eSampled;
        if (desc.isRenderTarget)
        {
            if (DepthStencilFormatMap.contains(desc.format))
            {
                usage |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
            }
            else
            {
                usage |= vk::ImageUsageFlagBits::eColorAttachment;
            }
        }
        if (desc.isUAV) usage |= vk::ImageUsageFlagBits::eStorage;

        return usage;
    }

    vk::SampleCountFlagBits PickImageSampleCount(const TextureDesc& desc)
    {
        switch (desc.sampleCount)
        {
            case 1: return vk::SampleCountFlagBits::e1;
            case 2: return vk::SampleCountFlagBits::e2;
            case 4: return vk::SampleCountFlagBits::e4;
            case 8: return vk::SampleCountFlagBits::e8;
            case 16: return vk::SampleCountFlagBits::e16;
            case 32: return vk::SampleCountFlagBits::e32;
            case 64: return vk::SampleCountFlagBits::e64;
            default: return vk::SampleCountFlagBits::e1;
        }
    }

    vk::ImageAspectFlags PickImageAspectFlags(vk::Format format)
    {
        switch (format)
        {
            case vk::Format::eD16Unorm:
            case vk::Format::eX8D24UnormPack32:
            case vk::Format::eD32Sfloat:
                return vk::ImageAspectFlagBits::eDepth;
            case vk::Format::eS8Uint:
                return vk::ImageAspectFlagBits::eStencil;
            case vk::Format::eD16UnormS8Uint:
            case vk::Format::eD24UnormS8Uint:
            case vk::Format::eD32SfloatS8Uint:
                return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
            default:
                return vk::ImageAspectFlagBits::eColor;
        }
    }

    vk::ImageAspectFlags PickSubresourceImageAspectFlags(vk::Format format,
        VulkanTexture::TextureSubresourceViewType viewType)
    {
        vk::ImageAspectFlags flags = PickImageAspectFlags(format);

        if ((flags & (vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil))
            == (vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil))
        {
            if (viewType == VulkanTexture::TextureSubresourceViewType::DepthOnly)
            {
                flags = flags & ~vk::ImageAspectFlagBits::eStencil;
            }
            else if (viewType == VulkanTexture::TextureSubresourceViewType::StencilOnly)
            {
                flags = flags & ~vk::ImageAspectFlagBits::eDepth;
            }
        }

        return flags;
    }

    vk::ImageCreateFlags PickImageCreateFlags(const TextureDesc& desc)
    {
        vk::ImageCreateFlags flags{};

        if (desc.dimension == TextureDimension::TextureCube || desc.dimension == TextureDimension::TextureCubeArray)
            flags |= vk::ImageCreateFlagBits::eCubeCompatible;

        if (desc.isTypeless)
            flags |= vk::ImageCreateFlagBits::eMutableFormat | vk::ImageCreateFlagBits::eExtendedUsage;

        if (desc.isTiled)
            flags |= vk::ImageCreateFlagBits::eSparseBinding | vk::ImageCreateFlagBits::eSparseResidency;

        return flags;
    }

    void FillTextureInfo(VulkanTexture* texture, const TextureDesc& desc)
    {
        texture->desc = desc;

        vk::ImageType type = ConvertTextureDimensionToImageType(desc.dimension);
        vk::Extent3D extent = PickImageExtent(desc);
        uint32_t layers = PickImageLayers(desc);
        vk::Format format = ConvertFormat(desc.format);
        vk::ImageUsageFlags usage = PickImageUsageFlags(desc);
        vk::SampleCountFlagBits samples = PickImageSampleCount(desc);
        vk::ImageCreateFlags createFlags = PickImageCreateFlags(desc);

        texture->imageCreateInfo = vk::ImageCreateInfo()
            .setImageType(type)
            .setExtent(extent)
            .setMipLevels(desc.mipLevels)
            .setArrayLayers(layers)
            .setFormat(format)
            .setTiling(vk::ImageTiling::eOptimal)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setUsage(usage)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setSamples(samples)
            .setFlags(createFlags);
    }

    VulkanTexture::~VulkanTexture()
    {
        // Temporary, probably
        //m_Context.device.destroyImageView(imageView, nullptr);
        for (auto& viewIter : subresourceViews)
        {
            auto& view = viewIter.second.imageView;
            m_Context.device.destroyImageView(view, nullptr);
            view = vk::ImageView();
        }
        subresourceViews.clear();

        vmaDestroyImage(m_Context.allocator, image, allocation);
    }

    TextureSubresourceView& VulkanTexture::GetSubresourceView(const TextureSubresourceSet& subresources,
        TextureDimension dimension, Format format, vk::ImageUsageFlags usage, TextureSubresourceViewType viewType)
    {
        if (dimension == TextureDimension::Unknown) dimension = desc.dimension;
        if (format == Format::UNKNOWN) format = desc.format;

        // If it's found, return it
        auto cacheKey = std::make_tuple(subresources, viewType, dimension, format, usage);
        auto it = subresourceViews.find(cacheKey);
        if (it != subresourceViews.end()) return it->second;

        // If not, create it
        auto iter_pair = subresourceViews.emplace(cacheKey, *this);
        auto& view = std::get<0>(iter_pair)->second;

        view.subresource = subresources;

        auto vkFormat = ConvertFormat(format);
        vk::ImageAspectFlags aspectFlags = PickSubresourceImageAspectFlags(vkFormat, viewType);
        view.subresourceRange = vk::ImageSubresourceRange(aspectFlags, subresources.baseMipLevel,
            subresources.numMipLevels, subresources.baseArraySlice, subresources.numArraySlices);

        vk::ImageViewType imageViewType = ConvertTextureDimensionToImageViewType(dimension);

        auto viewInfo = vk::ImageViewCreateInfo()
            .setImage(image)
            .setViewType(imageViewType)
            .setFormat(vkFormat)
            .setSubresourceRange(view.subresourceRange);

        auto usageInfo = vk::ImageViewUsageCreateInfo()
            .setUsage(usage);

        if (static_cast<uint32_t>(usage) != 0) viewInfo.setPNext(&usageInfo);

        if (viewType == TextureSubresourceViewType::StencilOnly)
        {
            // Check into this later
            viewInfo.components.setG(vk::ComponentSwizzle::eR);
        }

        const vk::Result result = m_Context.device.createImageView(&viewInfo, nullptr, &view.imageView);
        VK_CHECK(result);

        const std::string debugName = std::string("ImageView for: ") + desc.debugName;
        m_Context.nameVKObject(view.imageView, vk::ObjectType::eImageView, debugName.c_str());

        return view;
    }

    uint32_t VulkanTexture::GetNumSubresources() const
    {
        return desc.mipLevels * desc.arraySize;
    }

    uint32_t VulkanTexture::GetSubresourceIndex(uint32_t mipLevel, uint32_t arrayLayer) const
    {
        return mipLevel * desc.arraySize + arrayLayer;
    }
}
