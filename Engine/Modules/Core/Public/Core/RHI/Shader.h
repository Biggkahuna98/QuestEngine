#pragma once

#include "RHICommon.h"

namespace Quest
{
    enum class ShaderType : uint16_t
    {
        None,

        Vertex,
        Fragment,
        Compute,

        AllGraphics,
        All
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
        std::string entry_point = "main";
        ShaderType type = ShaderType::Vertex;
        ShaderLanguage language = ShaderLanguage::Slang;
    };

    class Shader : public Resource
    {
    public:
        virtual const ShaderDesc& GetDesc() const = 0;
    };

    using ShaderHandle = RefCountPtr<Shader>;
}