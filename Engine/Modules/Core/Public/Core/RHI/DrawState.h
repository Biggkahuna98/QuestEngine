#pragma once

#include "RHICommon.h"

#include "Pipeline.h"
#include "Buffer.h"
#include "ShaderBindings.h"

namespace Quest
{
    struct GraphicsState
    {
        GraphicsPipeline* pipeline = nullptr;
        BufferHandle vertexBuffer = nullptr;
        BufferHandle indexBuffer = nullptr;

        BindingSetHandle bindingSet = nullptr;
    };

    struct ComputeState
    {
        ComputePipeline* pipeline = nullptr;
    };

    struct DrawArguments
    {
        uint32_t vertexCount = 0;
        uint32_t instanceCount = 1;
        uint32_t startIndexLocation = 0;
        uint32_t startVertexIndex = 0;
        uint32_t startInstanceIndex = 0;
    };
}