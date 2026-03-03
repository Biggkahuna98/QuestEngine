#include "VulkanCommandList.h"

#include "VulkanContext.h"

namespace qrhi::vulkan
{
    VulkanCommandList::VulkanCommandList(CommandListDesc desc, const VulkanContext* context)
        : m_Context(context), m_Desc(desc)
    {
        //m_CurrentCommandBuffer = m_Context->GetQueue(m_Desc.type)->GetOrCreateTrackedCommandBuffer();
        m_TrackedCommandBuffers.push_back(m_Context->GetQueue(m_Desc.type)->GetOrCreateTrackedCommandBuffer());
        m_TrackedCommandBuffers.push_back(m_Context->GetQueue(m_Desc.type)->GetOrCreateTrackedCommandBuffer());
        m_TrackedCommandBuffers.push_back(m_Context->GetQueue(m_Desc.type)->GetOrCreateTrackedCommandBuffer());
    }

    VulkanCommandList::~VulkanCommandList()
    {
    }

    Quest::OpaqueObject VulkanCommandList::GetNativeType()
    {
        Quest::OpaqueObject obj;
        obj.pointer = static_cast<void*>(this);
        return obj;
    }

    void VulkanCommandList::SetDebugName(const std::string& name)
    {
        m_Context->SetDebugName(m_CurrentCommandBuffer->commandBuffer, vk::ObjectType::eCommandBuffer, name.c_str());
        std::string poolName = name + " - Pool";
        m_Context->SetDebugName(m_CurrentCommandBuffer->commandPool, vk::ObjectType::eCommandPool, poolName.c_str());
    }

    void VulkanCommandList::Open()
    {
        //m_CurrentCommandBuffer = m_Context->GetQueue(m_Desc.type)->GetOrCreateTrackedCommandBuffer();
        m_CurrentCommandBuffer = m_TrackedCommandBuffers[m_Context->GetFrameIndex()];
        m_CurrentCommandBuffer->commandBuffer.reset();
        m_CurrentCommandBuffer->commandBuffer.begin(vk::CommandBufferBeginInfo()
            .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    }

    void VulkanCommandList::Close()
    {
        m_CurrentCommandBuffer->commandBuffer.end();
    }

    void VulkanCommandList::SetGraphicsState(const GraphicsState& state)
    {
    }

    void VulkanCommandList::SetComputeState(const ComputeState& state)
    {
    }

    void VulkanCommandList::Draw(const DrawArguments& args)
    {
    }

    void VulkanCommandList::DrawIndexed(const DrawArguments& args)
    {
    }

    void VulkanCommandList::DispatchCompute(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ)
    {
    }

    void VulkanCommandList::ClearState()
    {
        m_CurrentGraphicsState = GraphicsState{};
        m_CurrentComputeState = ComputeState{};
    }
}
