#pragma once

#include "Vulkan_Backend.h"

namespace qrhi::vulkan
{
    class VulkanTexture;

    struct TextureSubresourceView
    {
        VulkanTexture& texture;
        TextureSubresourceSet subresource;

        vk::ImageView imageView = nullptr;
        vk::ImageSubresourceRange subresourceRange;

        TextureSubresourceView(VulkanTexture& texture)
            : texture(texture)
        {}

        TextureSubresourceView(const TextureSubresourceView&) = delete;

        bool operator==(const TextureSubresourceView& other) const
        {
            return &texture == &other.texture
                && subresource == other.subresource
                && imageView == other.imageView
                && subresourceRange == other.subresourceRange;
        }
    };

    class VulkanTexture : public MemoryResource, public qrhi::Texture
    {
    public:
        enum class TextureSubresourceViewType
        {
            AllAspects,
            DepthOnly,
            StencilOnly
        };

        using SubresourceViewKey = std::tuple<TextureSubresourceSet, TextureSubresourceViewType,
            TextureDimension, Format, vk::ImageUsageFlags>;

        struct Hash
        {
            std::size_t operator()(SubresourceViewKey const& s) const noexcept
            {
                const auto& [subresources, viewType, dimension, format, usage] = s;

                size_t hash = 0;

                hash_combine(hash, subresources.baseMipLevel);
                hash_combine(hash, subresources.numMipLevels);
                hash_combine(hash, subresources.baseArraySlice);
                hash_combine(hash, subresources.numArraySlices);
                hash_combine(hash, viewType);
                hash_combine(hash, dimension);
                hash_combine(hash, format);
                hash_combine(hash, uint32_t(usage));

                return hash;
            }
        };

        TextureDesc desc;

        vk::ImageCreateInfo imageCreateInfo;
        vk::Image image;

        // Map of the subresource views for this texture
        // multiple views may map to the same subresource
        std::unordered_map<SubresourceViewKey, TextureSubresourceView, VulkanTexture::Hash> subresourceViews;

        VulkanTexture(const VulkanContext& context)
            : m_Context(context) {}

        ~VulkanTexture() override;
        const TextureDesc& GetDesc() const override { return desc; }

        TextureSubresourceView& GetSubresourceView(const TextureSubresourceSet& subresources, TextureDimension dimension,
            Format format, vk::ImageUsageFlags usage, TextureSubresourceViewType viewType = TextureSubresourceViewType::AllAspects);

        uint32_t GetNumSubresources() const;
        uint32_t GetSubresourceIndex(uint32_t mipLevel, uint32_t arrayLayer) const;
    private:
        const VulkanContext& m_Context;
        //std::mutex m_Mutex;
    };

    vk::ImageType ConvertTextureDimensionToImageType(TextureDimension dimension);
    vk::ImageViewType ConvertTextureDimensionToImageViewType(TextureDimension dimension);
    vk::Extent3D PickImageExtent(const TextureDesc& desc);
    uint32_t PickImageLayers(const TextureDesc& desc);
    vk::ImageUsageFlags PickImageUsageFlags(const TextureDesc& desc);
    vk::SampleCountFlagBits PickImageSampleCount(const TextureDesc& desc);
    vk::ImageAspectFlags PickImageAspectFlags(vk::Format format);
    vk::ImageAspectFlags PickSubresourceImageAspectFlags(vk::Format format, VulkanTexture::TextureSubresourceViewType viewType);
    vk::ImageCreateFlags PickImageCreateFlags(const TextureDesc& desc);

    void FillTextureInfo(VulkanTexture* texture, const TextureDesc& desc);
}