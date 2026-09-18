#pragma once

#include "RHICommon.h"
#include "Buffer.h"
#include "Texture.h"
#include "Shader.h"
#include "ShaderBindings.h"
#include "Pipeline.h"
#include "CommandList.h"
#include "SyncObject.h"
#include "Swapchain.h"

#include <Core/RefCounting.h>
#include <Core/Window.h>

#include <vector>

namespace Quest
{
    struct DeviceDesc
    {
        GraphicsAPI graphicsAPI = GraphicsAPI::Vulkan;
        Window* window = nullptr;
        MessageCallback* messageCallback = nullptr;
        bool enableAPISpecificValidation = true;
    };

    // Graphics device, create gpu resources from this
    class Device : public RefCounted
    {
    public:
        virtual BufferHandle CreateBuffer(BufferDesc desc) = 0;
        virtual TextureHandle CreateTexture(TextureDesc desc) = 0;
        virtual ShaderHandle CreateShader(ShaderDesc desc) = 0;
        virtual ShaderHandle CreateShaderMutliDesc(std::vector<ShaderDesc> descs) = 0;
        virtual BindingLayoutHandle CreateBindingLayout(BindingLayoutDesc desc) = 0;
        virtual BindingLayoutHandle CreateBindlessLayout(BindlessLayoutDesc desc) = 0;
        virtual BindingSetHandle CreateBindingSet(const BindingSetDesc& desc, BindingLayout* layout) = 0;
        virtual GraphicsPipelineHandle CreateGraphicsPipeline(GraphicsPipelineDesc desc) = 0;
        virtual ComputePipelineHandle CreateComputePipeline(ComputePipelineDesc desc) = 0;
        virtual CommandListHandle CreateCommandList(CommandListDesc desc) = 0;
        virtual SyncObjectHandle CreateTimeline(uint64_t initialValue = 0) = 0;
        virtual SwapchainHandle CreateSwapchain(SwapchainDesc desc) = 0;

        virtual void* MapBuffer(Buffer* buffer) = 0;
        virtual void UnmapBuffer(Buffer* buffer) = 0;

        // Block until the GPU has finished all submitted work.
        virtual void WaitIdle() = 0;
    };

    using DeviceHandle = RefCountPtr<Device>;

    extern "C" Device* CreateDevice(DeviceDesc desc);
}