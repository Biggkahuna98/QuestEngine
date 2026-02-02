#pragma once

#include "Vulkan_Backend.h"

namespace qrhi::vulkan
{
    class VulkanShader : public qrhi::Shader
    {
    public:
        ShaderDesc desc;

        vk::ShaderModule shaderModule;
        vk::ShaderStageFlagBits stageFlagBits{};

        VulkanShader(const VulkanContext& context)
            : m_Context(context) {}
        ~VulkanShader() override;

        const ShaderDesc& GetDesc() const override { return desc; }
        void GetBytecode(const void** ppBytecode, size_t* pSize) const override;
    private:
        const VulkanContext& m_Context;
    };

    class VulkanShaderLibrary : public qrhi::ShaderLibrary
    {
    public:
        vk::ShaderModule shaderModule;

        VulkanShaderLibrary(const VulkanContext& context)
            : m_Context(context) {}
        ~VulkanShaderLibrary() override;

        ShaderHandle GetShader(const char* entryName, ShaderType type) override;
    private:
        const VulkanContext& m_Context;
    };
}