#pragma once

#include "Core/QuestExport.h"
#include "Utility/RefCounting.h"

#include "RHI_Common.h"
#include <cstdint>
#include <string>

namespace qrhi
{
    enum class TextureDimension : uint8_t
    {
        Unknown,
        Texture1D,
        Texture1DArray,
        Texture2D,
        Texture2DArray,
        TextureCube,
        TextureCubeArray,
        Texture2DMS,
        Texture2DMSArray,
        Texture3D
    };

    using MipLevel = uint32_t;
    using ArraySlice = uint32_t;

    struct TextureDesc
    {
        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t depth = 1;
        uint32_t arraySize = 1;
        uint32_t mipLevels = 1;
        uint32_t sampleCount = 1;
        uint32_t sampleQuality = 0;
        Format format = Format::UNKNOWN;
        TextureDimension dimension = TextureDimension::Texture2D;
        std::string debugName;

        bool isRenderTarget = false;
        bool isUAV = false;
        bool isVirtual = false;
        Color clearValue;
        bool useClearValue = false;
        bool keepInitialState = false;

        constexpr TextureDesc& setWidth(uint32_t value) { width = value; return *this; }
        constexpr TextureDesc& setHeight(uint32_t value) { height = value; return *this; }
        constexpr TextureDesc& setDepth(uint32_t value) { depth = value; return *this; }
        constexpr TextureDesc& setArraySize(uint32_t value) { arraySize = value; return *this; }
        constexpr TextureDesc& setMipLevels(uint32_t value) { mipLevels = value; return *this; }
        constexpr TextureDesc& setSampleCount(uint32_t value) { sampleCount = value; return *this; }
        constexpr TextureDesc& setSampleQuality(uint32_t value) { sampleQuality = value; return *this; }
        constexpr TextureDesc& setFormat(Format value) { format = value; return *this; }
        constexpr TextureDesc& setDimension(TextureDimension value) { dimension = value; return *this; }
        TextureDesc& setDebugName(const std::string& value) { debugName = value; return *this; }
        constexpr TextureDesc& setIsRenderTarget(bool value) { isRenderTarget = value; return *this; }
        constexpr TextureDesc& setIsUAV(bool value) { isUAV = value; return *this; }
        constexpr TextureDesc& setIsVirtual(bool value) { isVirtual = value; return *this; }
        constexpr TextureDesc& setClearValue(const Color& value) { clearValue = value; useClearValue = true; return *this; }
        constexpr TextureDesc& setUseClearValue(bool value) { useClearValue = value; return *this; }
        //constexpr TextureDesc& setInitialState(ResourceStates value) { initialState = value; return *this; }
        constexpr TextureDesc& setKeepInitialState(bool value) { keepInitialState = value; return *this; }
        //constexpr TextureDesc& setSharedResourceFlags(SharedResourceFlags value) { sharedResourceFlags = value; return *this; }
    };

    // Describes a 2D or 3D section of a single mip level, single array slice of a texture.
    struct TextureSlice
    {
        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t z = 0;
        // -1 means the entire dimension is part of the region
        // Resolve() below will translate these values into actual dimensions
        uint32_t width = -1;
        uint32_t height = -1;
        uint32_t depth = -1;

        MipLevel mipLevel = 0;
        ArraySlice arraySlice = 0;

        [[nodiscard]] QUEST_API TextureSlice Resolve(const TextureDesc& desc) const;

        constexpr TextureSlice& setOrigin(uint32_t vx = 0, uint32_t vy = 0, uint32_t vz = 0) { x = vx; y = vy; z = vz; return *this; }
        constexpr TextureSlice& setWidth(uint32_t value) { width = value; return *this; }
        constexpr TextureSlice& setHeight(uint32_t value) { height = value; return *this; }
        constexpr TextureSlice& setDepth(uint32_t value) { depth = value; return *this; }
        constexpr TextureSlice& setSize(uint32_t vx = -1, uint32_t vy = -1, uint32_t vz = -1) { width = vx; height = vy; depth = vz; return *this; }
        constexpr TextureSlice& setMipLevel(MipLevel level) { mipLevel = level; return *this; }
        constexpr TextureSlice& setArraySlice(ArraySlice slice) { arraySlice = slice; return *this; }
    };

    struct TextureSubresourceSet
    {
        static constexpr MipLevel AllMipLevels = -1;
        static constexpr ArraySlice AllArraySlices = -1;

        MipLevel baseMipLevel = 0;
        MipLevel numMipLevels = 1;
        ArraySlice baseArraySlice = 0;
        ArraySlice numArraySlices = 1;

        TextureSubresourceSet() = default;

        TextureSubresourceSet(MipLevel _baseMipLevel, MipLevel _numMipLevels, ArraySlice _baseArraySlice, ArraySlice _numArraySlices)
            : baseMipLevel(_baseMipLevel)
            , numMipLevels(_numMipLevels)
            , baseArraySlice(_baseArraySlice)
            , numArraySlices(_numArraySlices)
        {
        }

        [[nodiscard]] QUEST_API TextureSubresourceSet resolve(const TextureDesc& desc, bool singleMipLevel) const;
        [[nodiscard]] QUEST_API bool isEntireTexture(const TextureDesc& desc) const;

        bool operator==(const TextureSubresourceSet& other) const
        {
            return baseMipLevel == other.baseMipLevel &&
                numMipLevels == other.numMipLevels &&
                baseArraySlice == other.baseArraySlice &&
                numArraySlices == other.numArraySlices;
        }
        bool operator!=(const TextureSubresourceSet& other) const { return !(*this == other); }

        constexpr TextureSubresourceSet& setBaseMipLevel(MipLevel value) { baseMipLevel = value; return *this; }
        constexpr TextureSubresourceSet& setNumMipLevels(MipLevel value) { numMipLevels = value; return *this; }
        constexpr TextureSubresourceSet& setMipLevels(MipLevel base, MipLevel num) { baseMipLevel = base; numMipLevels = num; return *this; }
        constexpr TextureSubresourceSet& setBaseArraySlice(ArraySlice value) { baseArraySlice = value; return *this; }
        constexpr TextureSubresourceSet& setNumArraySlices(ArraySlice value) { numArraySlices = value; return *this; }
        constexpr TextureSubresourceSet& setArraySlices(ArraySlice base, ArraySlice num) { baseArraySlice = base; numArraySlices = num; return *this; }
    };

    static const TextureSubresourceSet AllSubresources = TextureSubresourceSet(0, TextureSubresourceSet::AllMipLevels, 0, TextureSubresourceSet::AllArraySlices);

    class Texture : public Resource
    {
    public:
        [[nodiscard]] virtual const TextureDesc& GetDesc() const = 0;
    };

    using TextureHandle = Quest::RefCountPtr<Texture>;
}

namespace std
{
    template<> struct hash<qrhi::TextureSubresourceSet>
    {
        std::size_t operator()(qrhi::TextureSubresourceSet const& s) const noexcept
        {
            size_t hash = 0;
            qrhi::hash_combine(hash, s.baseMipLevel);
            qrhi::hash_combine(hash, s.numMipLevels);
            qrhi::hash_combine(hash, s.baseArraySlice);
            qrhi::hash_combine(hash, s.numArraySlices);
            return hash;
        }
    };
}