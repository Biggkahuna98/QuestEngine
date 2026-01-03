#pragma once

#include "RHI_Common.h"
#include "Buffer.h"
#include "Texture.h"
#include "Sampler.h"
#include "DrawState.h"

namespace qrhi
{
    class Device;

    enum class QueueType : uint8_t
    {
        Graphics,
        Compute,
        Copy,

        Count
    };

    struct CommandListParameters
    {
        bool enableImmediateExecution = false;
        QueueType queueType = QueueType::Graphics;
    };

    class CommandList : public Resource
    {
    public:
        // Prepares the list for recording commands
        // must be called first before other commands are issued
        virtual void Begin() = 0;
        // Ends the recording, prepares for execution
        virtual void End() = 0;

        // Upload data to the buffer
        virtual void WriteBuffer(Buffer* buffer, const void* data, size_t dataSize, uint64_t dstOffsetBytes = 0) = 0;

        // Clear the buffer with the value
        virtual void ClearBuffer(Buffer* buffer, uint32_t clearValue) = 0;

        // Copy the buffer from src to dst
        virtual void CopyBuffer(Buffer* dst, uint64_t dstOffsetBytes, Buffer* src, uint64_t srcOffsetBytes, uint64_t dataSizeBytes) = 0;

        virtual void SetPushConstants(const void* data, size_t dataSizeBytes) = 0;

        virtual void SetGraphicsState(const GraphicsState& state) = 0;

        virtual void Draw(const DrawArguments& args) = 0;

        virtual void DrawIndexed(const DrawArguments& args) = 0;

        virtual void DrawIndirect(uint32_t offsetBytes, uint32_t drawCount = 1) = 0;

        virtual void DrawIndexedIndirect(uint32_t offsetBytes, uint32_t drawCount = 1) = 0;

        virtual void SetComputeState(const ComputeState& state) = 0;

        virtual void Dispatch(uint32_t groupsX, uint32_t groupsY = 1, uint32_t groupsZ = 1) = 0;

        virtual void DispatchIndirect(uint32_t offsetBytes) = 0;

        virtual void SetEnableAutomaticBarriers(bool enable) = 0;

        virtual void SetResourceStatesForBindingSet(BindingSet* bindingSet) = 0;

        virtual void SetResourceStatesForFramebuffer(Framebuffer* framebuffer) = 0;

        virtual void CommitBarriers() = 0;

        virtual Device* GetDevice() = 0;
    };

    using CommandListHandle = Quest::RefCountPtr<CommandList>;
}