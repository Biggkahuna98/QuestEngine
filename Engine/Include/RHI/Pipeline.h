#pragma once

#include "Framebuffer.h"
#include "RHI_Common.h"

#include "InputLayout.h"
#include "BindingLayout.h"
#include "Shader.h"

#include "Utility/StaticVector.h"

namespace qrhi
{
    enum class PrimitiveType : uint8_t
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

    struct SinglePassState
    {
        bool enabled = false;
        uint16_t renderTargetIndexOffset = 0;

        bool operator ==(const SinglePassState& rhs) const
        {
            return enabled == rhs.enabled
                && renderTargetIndexOffset == rhs.renderTargetIndexOffset;
        }

        bool operator !=(const SinglePassState& rhs) const { return !(*this == rhs); }

        constexpr SinglePassState& setEnabled(bool value) { enabled = value; return *this; }
        constexpr SinglePassState& setRenderTargetIndexOffset(uint16_t value) { renderTargetIndexOffset = value; return *this; }
    };

    using BindingLayoutVector = Quest::StaticVector<BindingLayoutHandle, c_MaxBindingLayouts>;

    struct RenderState
    {
        BlendState blendState;
        DepthStencilState depthStencilState;
        RasterState rasterState;
        SinglePassState singlePassState;

        constexpr RenderState& setBlendState(const BlendState& value) { blendState = value; return *this; }
        constexpr RenderState& setDepthStencilState(const DepthStencilState& value) { depthStencilState = value; return *this; }
        constexpr RenderState& setRasterState(const RasterState& value) { rasterState = value; return *this; }
        constexpr RenderState& setSinglePassStereoState(const SinglePassState& value) { singlePassState = value; return *this; }
    };

    struct GraphicsPipelineDesc
    {
        PrimitiveType primitiveType = PrimitiveType::TriangleList;
        uint32_t patchControlPoints = 0;
        InputLayoutHandle inputLayout;

        ShaderHandle vertexShader;
        ShaderHandle fragmentShader;

        RenderState renderState;

        std::vector<BindingLayoutHandle> bindingLayouts;

        GraphicsPipelineDesc& setPrimitiveType(PrimitiveType value) { primitiveType = value; return *this; }
        GraphicsPipelineDesc& setPatchControlPoints(uint32_t value) { patchControlPoints = value; return *this; }
        GraphicsPipelineDesc& setInputLayout(InputLayoutHandle value) { inputLayout = value; return *this; }
        GraphicsPipelineDesc& setVertexShader(ShaderHandle value) { vertexShader = value; return *this; }
        GraphicsPipelineDesc& setRenderState(const RenderState& value) { renderState = value; return *this; }
        GraphicsPipelineDesc& addBindingLayout(BindingLayoutHandle layout) { bindingLayouts.push_back(layout); return *this; }

    };

    class GraphicsPipeline : public Resource
    {
    public:
        [[nodiscard]] virtual const GraphicsPipelineDesc& GetDesc() const = 0;
        [[nodiscard]] virtual const FramebufferInfo& GetFramebufferInfo() const = 0;
    };

    using GraphicsPipelineHandle = Handle_T<GraphicsPipeline>;

    struct ComputePipelineDesc
    {
        ShaderHandle computeShader;
        std::vector<BindingLayoutHandle> bindingLayouts;

        ComputePipelineDesc& setComputeShader(ShaderHandle value) { computeShader = value; return *this; }
        ComputePipelineDesc& addBindingLayout(BindingLayoutHandle layout) { bindingLayouts.push_back(layout); return *this; }
    };

    class ComputePipeline : public Resource
    {
    public:
        [[nodiscard]] virtual const ComputePipelineDesc& GetDesc() const = 0;
    };

    using ComputePipelineHandle = Handle_T<ComputePipeline>;
}