#pragma once

#include "RHI_Common.h"
#include "Utility/RefCounting.h"

namespace qrhi
{
    enum class SamplerAddressMode : uint8_t
    {
        ClampToEdge,
        Repeat, // wrap
        ClampToBorder,
        MirrorRepeat,
        MirrorClampToEdge
    };

    enum class SamplerReductionType : uint8_t
    {
        Standard,
        Comparison,
        Minimum,
        Maximum
    };

    struct SamplerDesc
    {
        Color borderColor = 1.f;
        float maxAnisotropy = 1.f;
        float mipBias = 0.f;

        bool minFilter = true;
        bool magFilter = true;
        bool mipFilter = true;
        SamplerAddressMode addressU = SamplerAddressMode::ClampToEdge;
        SamplerAddressMode addressV = SamplerAddressMode::ClampToEdge;
        SamplerAddressMode addressW = SamplerAddressMode::ClampToEdge;
        SamplerReductionType reductionType = SamplerReductionType::Standard;

        SamplerDesc& setBorderColor(const Color& color) { borderColor = color; return *this; }
        SamplerDesc& setMaxAnisotropy(float value) { maxAnisotropy = value; return *this; }
        SamplerDesc& setMipBias(float value) { mipBias = value; return *this; }
        SamplerDesc& setMinFilter(bool enable) { minFilter = enable; return *this; }
        SamplerDesc& setMagFilter(bool enable) { magFilter = enable; return *this; }
        SamplerDesc& setMipFilter(bool enable) { mipFilter = enable; return *this; }
        SamplerDesc& setAllFilters(bool enable) { minFilter = magFilter = mipFilter = enable; return *this; }
        SamplerDesc& setAddressU(SamplerAddressMode mode) { addressU = mode; return *this; }
        SamplerDesc& setAddressV(SamplerAddressMode mode) { addressV = mode; return *this; }
        SamplerDesc& setAddressW(SamplerAddressMode mode) { addressW = mode; return *this; }
        SamplerDesc& setAllAddressModes(SamplerAddressMode mode) { addressU = addressV = addressW = mode; return *this; }
        SamplerDesc& setReductionType(SamplerReductionType type) { reductionType = type; return *this; }
    };

    class Sampler : public Resource
    {
    public:
        [[nodiscard]] virtual const SamplerDesc& GetDesc() const = 0;
    };

    using SamplerHandle = Handle_T<Sampler>;
}
