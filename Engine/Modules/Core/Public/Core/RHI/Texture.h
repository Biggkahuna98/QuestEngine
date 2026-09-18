#pragma once

#include "RHICommon.h"

namespace Quest
{
    struct TextureDesc
    {

    };

    class Texture : public Resource
    {
    public:
        virtual const TextureDesc& GetDesc() const = 0;
    };

    using TextureHandle = RefCountPtr<Texture>;
}