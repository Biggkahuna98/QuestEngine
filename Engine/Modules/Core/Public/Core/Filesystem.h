#pragma once
#include "Core/Core.h"

#include <filesystem>
#include <string>
#include <vector>

namespace Quest::Utils
{
    inline const CORE_API std::filesystem::path ResourcesPath{QE_RESOURCES_FOLDER};
    inline const CORE_API std::filesystem::path ShaderPath{ResourcesPath / "Shaders/"};
    inline const CORE_API std::filesystem::path ShaderCachePath{ResourcesPath / "ShaderCache/"};
    inline const CORE_API std::filesystem::path ModelsPath{ResourcesPath / "Models/"};
    inline const CORE_API std::filesystem::path TexturesPath{ResourcesPath / "Textures/"};

    CORE_API void CreateDirectory(const std::filesystem::path& path);
    CORE_API std::vector<std::string> GetFilesInDirectory(const std::filesystem::path& path);
    CORE_API bool FileExists(const std::string fileName, const std::filesystem::path& path);
    CORE_API bool IsFileNewer(const std::filesystem::path& path, const std::filesystem::path& otherPath); // path newer than otherPath -> true
    CORE_API std::string GetFileExtension(const std::string& fileName);
    CORE_API std::string GetFileNameWithoutExtension(const std::string& fileName);

    // Shader helpers
    CORE_API std::string ConvertShaderNameToCacheName(std::string shaderName);
    CORE_API std::vector<std::string> GetShadersToCompile();
    CORE_API std::string LoadShaderFromFile(const std::string& shaderName);
    CORE_API void WriteSPIRVToCache(const std::string& shaderName, const std::vector<uint32_t>& spirv);
    CORE_API std::vector<uint32_t> LoadSPIRVFromCache(const std::string& shaderName);
}