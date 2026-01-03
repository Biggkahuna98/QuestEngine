#pragma once

#include "RHI_Common.h"
#include "CommandList.h"
#include "Buffer.h"
#include "Texture.h"
#include "Shader.h"
#include "Sampler.h"
#include "Framebuffer.h"
#include "InputLayout.h"
#include "DrawState.h"

namespace qrhi
{
    class Device : public Resource
    {
    public:
        virtual TextureHandle CreateTexture(const TextureDesc& desc) = 0;

        virtual BufferHandle CreateBuffer(const BufferDesc& desc) = 0;
        virtual void* MapBuffer(Buffer* buffer, CPUAccessMode cpuAccess) = 0;
        virtual void UnmapBuffer(Buffer* buffer) = 0;

        virtual ShaderHandle CreateShader(const ShaderDesc& desc, const void* binary, size_t binarySize) = 0;
        virtual ShaderLibraryHandle CreateShaderLibrary(const void* binary, size_t binarySize) = 0;

        virtual SamplerHandle CreateSampler(const SamplerDesc& desc) = 0;

        virtual InputLayoutHandle CreateInputLayout(const VertexAttributeDesc* desc, uint32_t attributeCount, Shader* vertexShader);

        virtual GraphicsAPI GetGraphicsAPI() = 0;

        virtual FramebufferHandle CreateFramebuffer(const FramebufferDesc& desc) = 0;

        virtual GraphicsPipelineHandle CreateGraphicsPipeline(const GraphicsPipelineDesc& desc, Framebuffer* framebuffer) = 0;

        virtual ComputePipelineHandle CreateComputePipeline(const ComputePipelineDesc& desc) = 0;

        virtual BindingLayoutHandle CreateBindingLayout(const BindingLayoutDesc& desc) = 0;
        virtual BindingLayoutHandle CreateBindlessLayout(const BindlessLayoutDesc& desc) = 0;

        virtual BindingSetHandle CreateBindingSet(const BindingSetDesc& desc, BindingLayout* layout) = 0;

        virtual CommandListHandle CreateCommandList(const CommandListParameters& params = CommandListParameters()) = 0;
        virtual uint64_t ExecuteCommandList(CommandList* commandList, QueueType queue = QueueType::Graphics) = 0;
        virtual void QueueWaitForCommandList(QueueType waitQueue, QueueType executionQueue, uint64_t instance) = 0;

        // true if success, false if problem
        virtual bool WaitForIdle() = 0;
    };

    using DeviceHandle = Quest::RefCountPtr<Device>;
}