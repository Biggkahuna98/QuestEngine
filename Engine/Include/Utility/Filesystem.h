#pragma once
#include "Core/Core.h"

#include <filesystem>
#include <string>
#include <vector>

namespace Quest::Utils
{
    inline const QUEST_API std::filesystem::path ResourcesPath{QE_RESOURCES_FOLDER};
    inline const QUEST_API std::filesystem::path ShaderPath{ResourcesPath / "Shaders/"};
    inline const QUEST_API std::filesystem::path ShaderCachePath{ResourcesPath / "ShaderCache/"};
    inline const QUEST_API std::filesystem::path ModelsPath{ResourcesPath / "Models/"};
    inline const QUEST_API std::filesystem::path TexturesPath{ResourcesPath / "Textures/"};

    QUEST_API void CreateDirectory(const std::filesystem::path& path);
    QUEST_API std::vector<std::string> GetFilesInDirectory(const std::filesystem::path& path);
    QUEST_API bool FileExists(const std::string fileName, const std::filesystem::path& path);
    QUEST_API bool IsFileNewer(const std::filesystem::path& path, const std::filesystem::path& otherPath); // path newer than otherPath -> true
    QUEST_API std::string GetFileExtension(const std::string& fileName);
    QUEST_API std::string GetFileNameWithoutExtension(const std::string& fileName);

    // Shader helpers
    QUEST_API std::string ConvertShaderNameToCacheName(std::string shaderName);
    QUEST_API std::vector<std::string> GetShadersToCompile();
    QUEST_API std::string LoadShaderFromFile(const std::string& shaderName);
    QUEST_API void WriteSPIRVToCache(const std::string& shaderName, const std::vector<uint32_t>& spirv);
    QUEST_API std::vector<uint32_t> LoadSPIRVFromCache(const std::string& shaderName);
}