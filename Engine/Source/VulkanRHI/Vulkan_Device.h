#pragma once

#include "Vulkan_Backend.h"

#include <array>

namespace qrhi::vulkan
{
    class VulkanDevice : public qrhi::Device
    {
    public:
        VulkanDevice(const DeviceDesc& desc, const VulkanContext& context);
        ~VulkanDevice() override;

        Queue* GetQueue(QueueType queue) const { return m_Context.queues[static_cast<uint32_t>(queue)].get(); }

        // Device interface implementation
        TextureHandle CreateTexture(const TextureDesc& desc) override;

        BufferHandle CreateBuffer(const BufferDesc& desc) override;
        void* MapBuffer(qrhi::Buffer* buffer, CPUAccessMode cpuAccess) override;
        void UnmapBuffer(qrhi::Buffer* buffer) override;

        ShaderHandle CreateShader(const ShaderDesc& desc, const void* binary, size_t binarySize) override;
        ShaderLibraryHandle CreateShaderLibrary(const void* binary, size_t binarySize) override;

        SamplerHandle CreateSampler(const SamplerDesc& desc) override;

        InputLayoutHandle CreateInputLayout(const VertexAttributeDesc* desc, uint32_t attributeCount, Shader* vertexShader) override;

        FramebufferHandle CreateFramebuffer(const FramebufferDesc& desc) override;

        GraphicsPipelineHandle CreateGraphicsPipeline(const GraphicsPipelineDesc& desc, Framebuffer* framebuffer) override;

        ComputePipelineHandle CreateComputePipeline(const ComputePipelineDesc& desc) override;

        BindingLayoutHandle CreateBindingLayout(const BindingLayoutDesc& desc) override;
        BindingLayoutHandle CreateBindlessLayout(const BindlessLayoutDesc& desc) override;

        BindingSetHandle CreateBindingSet(const BindingSetDesc& desc, BindingLayout* layout) override;

        CommandListHandle CreateCommandList(const CommandListParameters& params = CommandListParameters()) override;
        uint64_t ExecuteCommandList(CommandList* commandList, QueueType queueType = QueueType::Graphics) override;
        void QueueWaitForCommandList(QueueType waitQueue, QueueType executionQueue, uint64_t instance) override;

        GraphicsAPI GetGraphicsAPI() override { return GraphicsAPI::Vulkan; }

        bool WaitForIdle() override;

        // Vulkan specific
        VkSemaphore GetQueueSemaphore(QueueType queueType);
        void QueueWaitForSemaphore(QueueType queueType, VkSemaphore semaphore, uint64_t value);
        void QueueSignalSemaphore(QueueType queueType, VkSemaphore semaphore, uint64_t value);
        uint64_t GetQueueCompletedSemaphoreValue(QueueType queueType);

    private:
        const VulkanContext& m_Context;

        void* MapBufferInternal(qrhi::Buffer* buffer, CPUAccessMode cpuAccess, uint64_t offset, size_t size) const;
    };
}