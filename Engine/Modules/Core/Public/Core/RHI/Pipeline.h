#pragma once

#include "RHICommon.h"

#include "Shader.h"
#include "ShaderBindings.h"

#include <Core/Containers/StaticVector.h>

namespace Quest
{
enum class PrimitiveType
    {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
        TriangleFan,
        TriangleListWithAdjacency,
        TriangleStripWithAdjacency,
        PatchList
    };

    enum class FillMode
    {
        Solid,
        Wireframe
    };

    enum class CullMode
    {
        Back,
        Front,
        None
    };

    using BindingLayoutVector = StaticVector<BindingLayoutHandle, c_MaxBindingLayouts>;

    struct GraphicsPipelineDesc
    {
        PrimitiveType primitiveType = PrimitiveType::TriangleList;
        FillMode fillMode = FillMode::Solid;

        ShaderHandle vertexShader;
        ShaderHandle fragmentShader;

        bool uberShader = true;

        BindingLayoutVector bindingLayouts;

        GraphicsPipelineDesc& SetPrimitiveType(PrimitiveType type) { primitiveType = type; return *this; }
        GraphicsPipelineDesc& SetFillMode(FillMode f) { fillMode = f; return *this; }
        GraphicsPipelineDesc& SetVertexShader(ShaderHandle shader) { vertexShader = shader; return *this; }
        GraphicsPipelineDesc& SetFragmentShader(ShaderHandle shader) { fragmentShader = shader; return *this; }
        GraphicsPipelineDesc& AddBindingLayout(BindingLayout* layout) { bindingLayouts.push_back(layout); return *this; }
    };

    class GraphicsPipeline : public Resource
    {
    public:
        virtual const GraphicsPipelineDesc& GetDesc() const = 0;
    };

    using GraphicsPipelineHandle = RefCountPtr<GraphicsPipeline>;

    struct ComputePipelineDesc
    {
        ShaderHandle computeShader;
        BindingLayoutVector bindingLayouts;

        ComputePipelineDesc& SetComputeShader(ShaderHandle shader) { computeShader = shader; return *this; }
        ComputePipelineDesc& AddBindingLayout(BindingLayout* layout) { bindingLayouts.push_back(layout); return *this; }
    };

    class ComputePipeline : public Resource
    {
    public:
        virtual const ComputePipelineDesc& GetDesc() const = 0;
    };

    using ComputePipelineHandle = RefCountPtr<ComputePipeline>;
}