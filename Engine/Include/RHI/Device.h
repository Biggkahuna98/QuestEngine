#pragma once

#include "QRHI.h"

namespace qrhi
{
    // Graphics device, create gpu resources from this
    class Device : public Quest::RefCounted
    {
    public:
        virtual ~Device() = default;

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

        virtual void* MapBuffer(Buffer* buffer) = 0;
        virtual void UnmapBuffer(Buffer* buffer) = 0;
    };

    using DeviceHandle = Quest::RefCountPtr<Device>;
}