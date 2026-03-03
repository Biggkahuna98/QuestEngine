#pragma once

#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "RHI/QRHI.h"

namespace qrhi::vulkan
{
    class VulkanShader : public Shader
    {
    public:
        VulkanShader(ShaderDesc desc, const VulkanContext* context);
        ~VulkanShader() override;

        Quest::OpaqueObject GetNativeType() override;
        void SetDebugName(const std::string& name) override;
        const ShaderDesc& GetDesc() const override;

        vk::ShaderModule GetShaderModule() const { return m_ShaderModule; }
    private:
        const VulkanContext* m_Context;

        ShaderDesc desc;
        vk::ShaderModule m_ShaderModule;
    };
}
