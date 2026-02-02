#pragma once

#include "Vulkan_Backend.h"
#include "RHI/CommandList.h"
#include "Vulkan_Device.h"

namespace qrhi::vulkan
{
    class VulkanCommandList : public qrhi::CommandList
    {
    public:
        VulkanCommandList(VulkanDevice* device, const VulkanContext& context, const CommandListParameters& parameters);
        ~VulkanCommandList() override;

        void Begin() override;
        void End() override;

        void ClearTexture(Texture* t, TextureSubresourceSet subresources, const Color& clearColor) override;
        void ClearDepthStencilTexture(Texture* t, TextureSubresourceSet subresources, bool clearDepth, float depth,
            bool clearStencil, uint8_t stencil) override;
        void ClearTextureUInt(Texture* t, TextureSubresourceSet subresources, uint32_t clearColor) override;
        void CopyTexture(Texture* src, const TextureSlice& srcSlice, Texture* dst,
            const TextureSlice& dstSlice) override;
        void WriteTexture(Texture* dst, uint32_t arraySlice, uint32_t mipLevel, const void* data, size_t x,
            size_t y) override;

        void WriteBuffer(qrhi::Buffer* buffer, const void* data, size_t dataSize, uint64_t dstOffsetBytes) override;
        void ClearBuffer(qrhi::Buffer* buffer, uint32_t clearValue) override;
        void CopyBuffer(qrhi::Buffer* dst, uint64_t dstOffsetBytes, qrhi::Buffer* src, uint64_t srcOffsetBytes,
            uint64_t dataSizeBytes) override;

        void SetPushConstants(const void* data, size_t dataSizeBytes) override;

        void SetGraphicsState(const GraphicsState& state) override;
        void Draw(const DrawArguments& args) override;
        void DrawIndexed(const DrawArguments& args) override;
        void DrawIndirect(uint32_t offsetBytes, uint32_t drawCount) override;
        void DrawIndexedIndirect(uint32_t offsetBytes, uint32_t drawCount) override;

        void SetComputeState(const ComputeState& state) override;
        void Dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ) override;
        void DispatchIndirect(uint32_t offsetBytes) override;

        void SetEnableAutomaticBarriers(bool enable) override;
        void SetResourceStatesForBindingSet(BindingSet* bindingSet) override;
        void SetResourceStatesForFramebuffer(Framebuffer* framebuffer) override;

        void CommitBarriers() override;

        qrhi::Device* GetDevice() override { return m_Device; }

        TrackedCommandBufferPtr GetCurrentCommandBuffer() const { return m_CurrentCommandBuffer; }
    private:
        VulkanDevice* m_Device;
        const VulkanContext& m_Context;

        CommandListParameters m_CommandListParameters;

        TrackedCommandBufferPtr m_CurrentCommandBuffer = nullptr;

        vk::PipelineLayout m_CurrentPipelineLayout;
        vk::ShaderStageFlags m_CurrentPushConstantVisibility;
        GraphicsState m_CurrentGraphicsState{};
        ComputeState m_CurrentComputeState{};
    };
}