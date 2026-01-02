#pragma once

#include "RHI_Common.h"
#include "Buffer.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "BindingLayout.h"

namespace qrhi
{
    struct VertexBufferBinding
    {
        Buffer* buffer = nullptr;
        uint32_t slot;
        uint64_t offset;

        bool operator ==(const VertexBufferBinding& b) const
        {
            return buffer == b.buffer
                && slot == b.slot
                && offset == b.offset;
        }
        bool operator !=(const VertexBufferBinding& b) const { return !(*this == b); }

        VertexBufferBinding& setBuffer(Buffer* value) { buffer = value; return *this; }
        VertexBufferBinding& setSlot(uint32_t value) { slot = value; return *this; }
        VertexBufferBinding& setOffset(uint64_t value) { offset = value; return *this; }
    };

    struct IndexBufferBinding
    {
        Buffer* buffer = nullptr;
        Format format;
        uint32_t offset;

        bool operator ==(const IndexBufferBinding& b) const
        {
            return buffer == b.buffer
                && format == b.format
                && offset == b.offset;
        }
        bool operator !=(const IndexBufferBinding& b) const { return !(*this == b); }

        IndexBufferBinding& setBuffer(Buffer* value) { buffer = value; return *this; }
        IndexBufferBinding& setFormat(Format value) { format = value; return *this; }
        IndexBufferBinding& setOffset(uint32_t value) { offset = value; return *this; }
    };

    struct GraphicsState
    {
        GraphicsPipeline* pipeline = nullptr;
        Framebuffer* framebuffer = nullptr;
        ViewportState viewport;
        Color blendConstantColor{};
        uint8_t dynamicStencilRefValue = 0;

        std::vector<BindingSet> bindings;

        std::vector<VertexBufferBinding> vertexBuffers;
        IndexBufferBinding indexBuffer;

        Buffer* indirectParams = nullptr;

        GraphicsState& setPipeline(GraphicsPipeline* value) { pipeline = value; return *this; }
        GraphicsState& setFramebuffer(Framebuffer* value) { framebuffer = value; return *this; }
        GraphicsState& setViewport(const ViewportState& value) { viewport = value; return *this; }
        GraphicsState& setBlendColor(const Color& value) { blendConstantColor = value; return *this; }
        GraphicsState& setDynamicStencilRefValue(uint8_t value) { dynamicStencilRefValue = value; return *this; }
        GraphicsState& addBindingSet(BindingSet* value) { bindings.push_back(value); return *this; }
        GraphicsState& addVertexBuffer(const VertexBufferBinding& value) { vertexBuffers.push_back(value); return *this; }
        GraphicsState& setIndexBuffer(const IndexBufferBinding& value) { indexBuffer = value; return *this; }
        GraphicsState& setIndirectParams(Buffer* value) { indirectParams = value; return *this; }
    };

    struct DrawArguments
    {
        uint32_t vertexCount = 0;
        uint32_t instanceCount = 1;
        uint32_t startIndexLocation = 0;
        uint32_t startVertexLocation = 0;
        uint32_t startInstanceLocation = 0;

        constexpr DrawArguments& setVertexCount(uint32_t value) { vertexCount = value; return *this; }
        constexpr DrawArguments& setInstanceCount(uint32_t value) { instanceCount = value; return *this; }
        constexpr DrawArguments& setStartIndexLocation(uint32_t value) { startIndexLocation = value; return *this; }
        constexpr DrawArguments& setStartVertexLocation(uint32_t value) { startVertexLocation = value; return *this; }
        constexpr DrawArguments& setStartInstanceLocation(uint32_t value) { startInstanceLocation = value; return *this; }
    };

    struct DrawIndirectArguments
    {
        uint32_t vertexCount = 0;
        uint32_t instanceCount = 1;
        uint32_t startVertexLocation = 0;
        uint32_t startInstanceLocation = 0;

        constexpr DrawIndirectArguments& setVertexCount(uint32_t value) { vertexCount = value; return *this; }
        constexpr DrawIndirectArguments& setInstanceCount(uint32_t value) { instanceCount = value; return *this; }
        constexpr DrawIndirectArguments& setStartVertexLocation(uint32_t value) { startVertexLocation = value; return *this; }
        constexpr DrawIndirectArguments& setStartInstanceLocation(uint32_t value) { startInstanceLocation = value; return *this; }
    };

    struct DrawIndexedIndirectArguments
    {
        uint32_t indexCount = 0;
        uint32_t instanceCount = 1;
        uint32_t startIndexLocation = 0;
        int32_t  baseVertexLocation = 0;
        uint32_t startInstanceLocation = 0;

        constexpr DrawIndexedIndirectArguments& setIndexCount(uint32_t value) { indexCount = value; return *this; }
        constexpr DrawIndexedIndirectArguments& setInstanceCount(uint32_t value) { instanceCount = value; return *this; }
        constexpr DrawIndexedIndirectArguments& setStartIndexLocation(uint32_t value) { startIndexLocation = value; return *this; }
        constexpr DrawIndexedIndirectArguments& setBaseVertexLocation(int32_t value) { baseVertexLocation = value; return *this; }
        constexpr DrawIndexedIndirectArguments& setStartInstanceLocation(uint32_t value) { startInstanceLocation = value; return *this; }
    };

    struct ComputeState
    {
        ComputePipeline* pipeline = nullptr;

        std::vector<BindingSet> bindings;

        Buffer* indirectParams = nullptr;

        ComputeState& setPipeline(ComputePipeline* value) { pipeline = value; return *this; }
        ComputeState& addBindingSet(BindingSet* value) { bindings.push_back(value); return *this; }
        ComputeState& setIndirectParams(Buffer* value) { indirectParams = value; return *this; }
    };

    struct DispatchIndirectArguments
    {
        uint32_t groupsX = 1;
        uint32_t groupsY = 1;
        uint32_t groupsZ = 1;

        constexpr DispatchIndirectArguments& setGroupsX(uint32_t value) { groupsX = value; return *this; }
        constexpr DispatchIndirectArguments& setGroupsY(uint32_t value) { groupsY = value; return *this; }
        constexpr DispatchIndirectArguments& setGroupsZ(uint32_t value) { groupsZ = value; return *this; }
        constexpr DispatchIndirectArguments& setGroups2D(uint32_t x, uint32_t y) { groupsX = x; groupsY = y; return *this; }
        constexpr DispatchIndirectArguments& setGroups3D(uint32_t x, uint32_t y, uint32_t z) { groupsX = x; groupsY = y; groupsZ = z; return *this; }
    };
}