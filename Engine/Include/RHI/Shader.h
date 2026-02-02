#pragma once

#include "Utility/RefCounting.h"

#include "RHI_Common.h"
#include <cstdint>
#include <string>

namespace qrhi
{
    enum class ShaderType : uint16_t // set to vulkan values
    {
        None = 0x0000,
        Compute = 0x0020,
        Vertex = 0x0001,
        Fragment = 0x0010,
        AllGraphics = 0x00DF,
        All = 0x3FFF
    };

    QRHI_ENUM_CLASS_FLAG_OPERATORS(ShaderType)

    struct ShaderDesc
    {
        ShaderType type = ShaderType::None;
        std::string debugName;
        std::string entrypoint = "main";

        constexpr ShaderDesc& setType(ShaderType value) { type = value; return *this; }
        ShaderDesc& setDebugName(const std::string& value) { debugName = value; return *this; }
        ShaderDesc& setEntrypoint(const char* value) { entrypoint = value; return *this; }
        constexpr ShaderDesc& setEntrypoint(const std::string& value) { entrypoint = value; return *this; }
    };

    class Shader : public Resource
    {
    public:
        [[nodiscard]] virtual const ShaderDesc& GetDesc() const = 0;
        virtual void GetBytecode(const void** ppBytecode, size_t* pSize) const = 0;
    };

    using ShaderHandle = Handle_T<Shader>;

    class ShaderLibrary : public Resource
    {
    public:
        virtual ShaderHandle GetShader(const char* entryName, ShaderType type) = 0;
    };

    using ShaderLibraryHandle = Handle_T<ShaderLibrary>;
}