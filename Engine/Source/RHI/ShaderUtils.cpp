#include "RHI/ShaderUtils.h"
#include "Core/Log.h"
#include "Utility/Filesystem.h"

#include <array>
#include <fstream>

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

namespace qrhi
{
    Slang::ComPtr<slang::IGlobalSession> g_SlangGlobalSession;
    Slang::ComPtr<slang::ISession> g_SlangSession; // SPIRV session
    bool sessionInit = false;

    void InitSlangSession()
    {
        if (sessionInit)
            return;

        SlangResult res = slang::createGlobalSession(g_SlangGlobalSession.writeRef());
        if (SLANG_FAILED(res) || !g_SlangGlobalSession)
        {
            LOG_ERROR("Failed to create Slang global session");
            return;
        }

        slang::SessionDesc sessionDesc{};

        slang::TargetDesc targetDesc{};
        targetDesc.format = SLANG_SPIRV;
        targetDesc.profile = g_SlangGlobalSession->findProfile("spirv_1_5");
        if (targetDesc.profile == SLANG_PROFILE_UNKNOWN)
        {
            LOG_ERROR("Failed to find SPIR-V profile");
            return;
        }

        sessionDesc.targets = &targetDesc;
        sessionDesc.targetCount = 1;

        std::array<slang::CompilerOptionEntry, 1> options = {
            {
                slang::CompilerOptionName::EmitSpirvDirectly,
                { slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}
            }
        };

        sessionDesc.compilerOptionEntries = options.data();
        sessionDesc.compilerOptionEntryCount = options.size();

        res = g_SlangGlobalSession->createSession(sessionDesc, g_SlangSession.writeRef());

        if (SLANG_FAILED(res) || !g_SlangSession)
        {
            LOG_ERROR("Failed to create Slang session");
            return;
        }

        sessionInit = true;
    }

    inline void diagnoseIfNeeded(slang::IBlob *diagnosticBlob)
    {
        if (diagnosticBlob != nullptr)
        {
            LOG_ERROR("{}", static_cast<const char*>(diagnosticBlob->getBufferPointer()));
        }
    }

    void CompileShader(std::string_view shaderName, std::string_view shaderPath)
    {
        InitSlangSession();

        std::string path = QE_RESOURCES_FOLDER;
        path += "/";
        path += "Shaders/";
        path += shaderName;
        path += ".slang";

        LOG_DEBUG("Path: {}", path);

        Slang::ComPtr<slang::IModule> slangModule = nullptr;
        {
            Slang::ComPtr<slang::IBlob> diagnosticBlob;
            slangModule = g_SlangSession->loadModule(path.c_str(), diagnosticBlob.writeRef());
            diagnoseIfNeeded(diagnosticBlob);
            if (!slangModule)
            {
                LOG_ERROR("Failed to load module: {}", path);
                return;
            }
        }

        Slang::ComPtr<slang::IEntryPoint> entryPoint;
        {
            slangModule->findEntryPointByName("vertMain", entryPoint.writeRef());
            if (!entryPoint)
            {
                LOG_ERROR("Failed to find entry point: vertMain");
                return;
            }
        }

        Slang::ComPtr<slang::IEntryPoint> entryPoint2;
        {
            slangModule->findEntryPointByName("fragMain", entryPoint2.writeRef());
            if (!entryPoint2)
            {
                LOG_ERROR("Failed to find entry point: fragMain");
                return;
            }
        }

        std::array<slang::IComponentType*, 3> componentTypes = {
            slangModule,
            entryPoint,
            entryPoint2
        };

        Slang::ComPtr<slang::IComponentType> composedProgram;
        {
            Slang::ComPtr<slang::IBlob> diagnosticBlob;
            SlangResult result = g_SlangSession->createCompositeComponentType(
                componentTypes.data(),
                componentTypes.size(),
                composedProgram.writeRef(),
                diagnosticBlob.writeRef()
            );
            diagnoseIfNeeded(diagnosticBlob);
            if (SLANG_FAILED(result))
            {
                LOG_ERROR("Failed to create composite component type: {}", path);
                return;
            }
        }

        Slang::ComPtr<slang::IComponentType> program;
        {
            Slang::ComPtr<slang::IBlob> diagnosticBlob;
            SlangResult result = composedProgram->link(
                program.writeRef(),
                diagnosticBlob.writeRef()
            );
            diagnoseIfNeeded(diagnosticBlob);
            if (SLANG_FAILED(result))
            {
                LOG_ERROR("Failed to link component type: {}", path);
                return;
            }
        }

        Slang::ComPtr<ISlangBlob> spirv;
        {
            Slang::ComPtr<slang::IBlob> diagnosticBlob;
            SlangResult result = program->getTargetCode(
                0,
                spirv.writeRef(),
                diagnosticBlob.writeRef()
            );
            diagnoseIfNeeded(diagnosticBlob);
            if (SLANG_FAILED(result))
            {
                LOG_ERROR("Failed to get entry point code: {}", path);
                return;
            }
        }

        size_t byteSize = spirv->getBufferSize();
        if (byteSize % 4 != 0)
        {
            LOG_ERROR("SPIR-V bytecode is not a multiple of 4 bytes: {}", path);
            return;
        }

        std::vector<uint32_t> buffer(byteSize / 4);
        memcpy(buffer.data(), spirv->getBufferPointer(), byteSize);

        // Make sure the cache exists
        Quest::Utils::CreateDirectory(Quest::Utils::ShaderCachePath);

        std::string cacheName = shaderName.data();
        cacheName += ".spv";
        std::ofstream file{Quest::Utils::ShaderCachePath / cacheName, std::ios::binary};
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open shader cache file: {}", cacheName);
            return;
        }

        file.write(reinterpret_cast<const char*>(spirv->getBufferPointer()), byteSize);
        file.close();

        /*std::string cachePath = QE_RESOURCES_FOLDER;
        cachePath += "/ShaderCache/";
        cachePath += shaderName;
        cachePath += ".spv";

        LOG_DEBUG("Cache path: {}", cachePath);*/

    }

    std::vector<uint32_t> ReadShaderCache(std::string_view shaderName)
    {
        std::string cachePath = QE_RESOURCES_FOLDER;
        cachePath += "/ShaderCache/";
        Quest::Utils::CreateDirectory(cachePath);
        cachePath += shaderName;

        std::ifstream file(cachePath, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open shader cache file: {}", cachePath);
            return {};
        }

        std::streamsize size = file.tellg();
        if (size % 4 != 0)
        {
            LOG_ERROR("Shader cache file is not a multiple of 4 bytes: {}", cachePath);
        }

        std::vector<uint32_t> buffer(size);
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));
        file.close();

        return buffer;
    }
}
