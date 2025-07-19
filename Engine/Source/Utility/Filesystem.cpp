#include "Utility/Filesystem.h"
#include "Core/Log.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace QE::Utils
{
    void CreateDirectory(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path))
            std::filesystem::create_directories(path);
    }

    std::vector<std::string> GetFilesInDirectory(const std::filesystem::path &path)
    {
        std::vector<std::string> files{};

        if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
        {
            for (const auto &entry : std::filesystem::directory_iterator(path))
            {
                if (std::filesystem::is_regular_file(entry))
                    files.push_back(entry.path().filename().string());
            }
        }

        return files;
    }

    bool FileExists(const std::string fileName, const std::filesystem::path &path)
    {
        return std::filesystem::exists(path / fileName);
    }

    bool IsFileNewer(const std::filesystem::path &path, const std::filesystem::path &otherPath)
    {
        if (!std::filesystem::exists(path) || !std::filesystem::exists(otherPath))
            return false;

        auto time1 = std::filesystem::last_write_time(path);
        auto time2 = std::filesystem::last_write_time(otherPath);

        return time1 > time2;
    }

    std::string GetFileExtension(const std::string &fileName)
    {
        return fileName.substr(fileName.rfind('.') + 1);
    }

    std::string GetFileNameWithoutExtension(const std::string &fileName)
    {
        return fileName.substr(0, fileName.rfind('.'));
    }


    std::string ConvertShaderNameToCacheName(std::string shaderName)
    {
        std::filesystem::path shaderNamePath{shaderName};
        auto shaderExtension = shaderNamePath.extension().string().substr(1);

        // Get the name before the extension
        auto nameWithoutExtension = shaderName.substr(0, shaderName.rfind('.'));

        // Create Cache name
        auto cacheName = nameWithoutExtension + "-" + shaderExtension + ".spv";

        return cacheName;
    }


    std::vector<std::string> GetShadersToCompile()
    {
        std::vector<std::string> shaders{};

        auto shaderDirectoryFiles = GetFilesInDirectory(ShaderPath);
        auto shaderCacheDirectoryFiles = GetFilesInDirectory(ShaderCachePath);

        for (const auto &file : shaderDirectoryFiles)
        {
            // Found in the cache
            auto cacheName = ConvertShaderNameToCacheName(file);
            if (std::find(shaderCacheDirectoryFiles.begin(), shaderCacheDirectoryFiles.end(), cacheName) != shaderCacheDirectoryFiles.end())
            {
                if (IsFileNewer(ShaderPath / file, ShaderCachePath / cacheName))
                {
                    shaders.push_back(file);
                }
            } else // not in the cache
            {
                shaders.push_back(file);
            }
        }

        return shaders;
    }

    std::string LoadShaderFromFile(const std::string &shaderName)
    {
        std::ifstream file{ShaderPath / shaderName};
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open shader file: {}", shaderName);
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    }

    void WriteSPIRVToCache(const std::string &shaderName, const std::vector<uint32_t> &spirv)
    {
        std::ofstream file{ShaderCachePath / ConvertShaderNameToCacheName(shaderName), std::ios::binary};
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open shader cache file: {}", ConvertShaderNameToCacheName(shaderName));
            return;
        }

        file.write(reinterpret_cast<const char*>(spirv.data()), spirv.size() * sizeof(uint32_t));
        file.close();
    }

    std::vector<uint32_t> LoadSPIRVFromCache(const std::string &shaderName)
    {
        std::ifstream file{ShaderCachePath / ConvertShaderNameToCacheName(shaderName), std::ios::ate | std::ios::binary};
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open shader cache file: {}", ConvertShaderNameToCacheName(shaderName));
            return {};
        }

        std::streamsize size = file.tellg();
        if (size % 4 != 0)
        {
            LOG_ERROR("Shader cache file is not a multiple of 4 bytes: {}", ConvertShaderNameToCacheName(shaderName));
            return {};
        }

        file.seekg(0);
        std::vector<uint32_t> buffer(size / sizeof(uint32_t));
        if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
        {
            LOG_ERROR("Failed to read shader cache file: {}", ConvertShaderNameToCacheName(shaderName));
            return {};
        }
        file.close();

        if (buffer[0] != 0x07230203)
            LOG_ERROR("Invalid SPIR-V magic number: 0x{:08x}", buffer[0]);

        return buffer;
    }
}
