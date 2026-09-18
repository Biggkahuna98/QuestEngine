#pragma once

#include <string>
#include <vector>

namespace qrhi
{
    void CompileShader(std::string_view shaderName, std::string_view shaderPath);

    std::vector<uint32_t> ReadShaderCache(std::string_view shaderName);
}