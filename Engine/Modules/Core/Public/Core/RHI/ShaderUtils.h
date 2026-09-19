#pragma once

#include "Core/CoreExport.h"

#include <string>
#include <vector>

namespace qrhi
{
    CORE_API void CompileShader(std::string_view shaderName, std::string_view shaderPath);

    CORE_API std::vector<uint32_t> ReadShaderCache(std::string_view shaderName);
}