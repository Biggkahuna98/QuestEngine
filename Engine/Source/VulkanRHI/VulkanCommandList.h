#pragma once

#include "VulkanCommon.h"
#include "RHI/QRHI.h"

namespace qrhi::vulkan
{
    class VulkanCommandList : public CommandList
    {
    public:
        VulkanCommandList(CommandListDesc desc, const VulkanContext* context);
        ~VulkanCommandList() override;

        Quest::OpaqueObject GetNativeType() override;
        void SetDebugName(const std::string& name) override;
        const CommandListDesc& GetDesc() const override { return m_Desc; }

        void Open() override;
        void Close() override;
        void SetGraphicsState(const GraphicsState& state) override;
        void SetComputeState(const ComputeState& state) override;
        void Draw(const DrawArguments& args) override;
        void DrawIndexed(const DrawArguments& args) override;
        void DispatchCompute(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ) override;

        void ClearState() override;

        TrackedCommandBufferPtr GetCurrentCommandBuffer() { return m_CurrentCommandBuffer; }
        vk::CommandBuffer GetCommandBuffer() const { return m_CurrentCommandBuffer->commandBuffer; }
        vk::CommandPool GetCommandPool() const { return m_CurrentCommandBuffer->commandPool; }
    private:
        const VulkanContext* m_Context;
        CommandListDesc m_Desc;

        GraphicsState m_CurrentGraphicsState{};
        ComputeState m_CurrentComputeState{};

        TrackedCommandBufferPtr m_CurrentCommandBuffer;

        // For now, duplicate command buffers on a per-frame basis
        std::vector<TrackedCommandBufferPtr> m_TrackedCommandBuffers{3};
    };
}
