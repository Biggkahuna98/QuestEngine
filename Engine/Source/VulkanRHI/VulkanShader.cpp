#include "VulkanShader.h"

#include "VulkanContext.h"
#include "RHI/ShaderUtils.h"

namespace qrhi::vulkan
{
    VulkanShader::VulkanShader(ShaderDesc desc, const VulkanContext* context)
        : m_Context(context), desc(desc)
    {
        auto bytecode = ReadShaderCache(desc.name);
        if (bytecode.empty())
        {
            m_Context->logError("Failed to read shader file: " + desc.name);
            return;
        }

        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.codeSize = bytecode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());

        m_ShaderModule = m_Context->GetDevice().createShaderModule(createInfo);
    }

    VulkanShader::~VulkanShader()
    {
        m_Context->GetDevice().destroyShaderModule(m_ShaderModule);
    }

    Quest::OpaqueObject VulkanShader::GetNativeType()
    {
        Quest::OpaqueObject obj{};
        obj.pointer = static_cast<void*>(this);
        return obj;
    }

    void VulkanShader::SetDebugName(const std::string& name)
    {
        m_Context->SetDebugName(m_ShaderModule, vk::ObjectType::eShaderModule, name);
    }

    const ShaderDesc& VulkanShader::GetDesc() const
    {
        return desc;
    }
}
