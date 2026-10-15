#pragma once

#include "RHICommon.h"

#include "DrawState.h"

namespace Quest
{
    enum class QueueType
    {
        Graphics,
        Compute,
        Transfer,
        Present,

        Count
    };

    struct CommandListDesc
    {
        QueueType type = QueueType::Graphics;
    };

    class CommandList : public Resource
    {
    public:
        virtual const CommandListDesc& GetDesc() const = 0;

        virtual void Open() = 0;
        virtual void Close() = 0;

        virtual void SetGraphicsState(const GraphicsState& state) = 0;
        virtual void SetComputeState(const ComputeState& state) = 0;

        virtual void Draw(const DrawArguments& args) = 0;
        virtual void DrawIndexed(const DrawArguments& args) = 0;
        virtual void DispatchCompute(uint32_t groupsX, uint32_t groupsY = 1, uint32_t groupsZ = 1) = 0;

        virtual void ClearState() = 0;
    };

    using CommandListHandle = RefCountPtr<CommandList>;
}