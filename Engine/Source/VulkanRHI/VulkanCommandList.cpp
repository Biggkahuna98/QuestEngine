#include "VulkanCommandList.h"

#include "VulkanBindingSet.h"
#include "VulkanPipeline.h"
#include "VulkanBuffer.h"

#include "VulkanContext.h"

namespace qrhi::vulkan
{
    // temporary
    // Reverse-Z perspective matrix
    glm::mat4 ReversedZPerspective(float fovYRadians, float aspect, float zNear)
    {
        float f = 1.0f / tan(fovYRadians / 2.0f);

        glm::mat4 result(0.0f);
        result[0][0] = f / aspect;
        result[1][1] = f;
        result[2][2] = 0.0f;
        result[2][3] = -1.0f;
        result[3][2] = zNear;
        return result;
    }

    VulkanCommandList::VulkanCommandList(CommandListDesc desc, VulkanContext* context)
        : m_Context(context), m_Desc(desc)
    {
        //m_CurrentCommandBuffer = m_Context->GetQueue(m_Desc.type)->GetOrCreateTrackedCommandBuffer();
        m_TrackedCommandBuffers.reserve(3);
        m_TrackedCommandBuffers.push_back(m_Context->GetQueue(m_Desc.type)->GetOrCreateTrackedCommandBuffer());
        m_TrackedCommandBuffers.push_back(m_Context->GetQueue(m_Desc.type)->GetOrCreateTrackedCommandBuffer());
        m_TrackedCommandBuffers.push_back(m_Context->GetQueue(m_Desc.type)->GetOrCreateTrackedCommandBuffer());
    }

    VulkanCommandList::~VulkanCommandList()
    {
    }

    Quest::OpaqueObject VulkanCommandList::GetNativeType()
    {
        Quest::OpaqueObject obj{};
        obj.pointer = static_cast<void*>(this);
        return obj;
    }

    void VulkanCommandList::SetDebugName(const std::string& name)
    {
        m_Context->SetDebugName(m_CurrentCommandBuffer->commandBuffer, vk::ObjectType::eCommandBuffer, name.c_str());
        std::string poolName = name + " - Pool";
        m_Context->SetDebugName(m_CurrentCommandBuffer->commandPool, vk::ObjectType::eCommandPool, poolName.c_str());
    }

    void VulkanCommandList::Open()
    {
        //m_CurrentCommandBuffer = m_Context->GetQueue(m_Desc.type)->GetOrCreateTrackedCommandBuffer();
        m_CurrentCommandBuffer = m_TrackedCommandBuffers[m_Context->GetFrameIndex()];
        m_CurrentCommandBuffer->commandBuffer.reset();
        m_CurrentCommandBuffer->commandBuffer.begin(vk::CommandBufferBeginInfo()
            .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

        // From here down to the end should all probably be render pass related, will keep here for now.
        // Setup the framebuffer to draw to (swapchain image for now)
        m_Context->TransitionImage(m_CurrentCommandBuffer->commandBuffer,
            m_Context->GetSwapchainImages().at(m_Context->GetSwapchainIndex()),
            vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);

        // Set up the color attachment
        vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
        vk::RenderingAttachmentInfo attachmentInfo = {
            .imageView = m_Context->GetSwapchainImageViews().at(m_Context->GetSwapchainIndex()),
            .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
            .loadOp = vk::AttachmentLoadOp::eClear,
            .storeOp = vk::AttachmentStoreOp::eStore,
            .clearValue = clearColor
        };

        // Set up the rendering info
        vk::RenderingInfo renderingInfo = {
            .renderArea = { .offset = { 0, 0 }, .extent = m_Context->GetSwapchainExtent() },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &attachmentInfo
        };

        // Begin rendering, yay!
        m_CurrentCommandBuffer->commandBuffer.beginRendering(renderingInfo);
    }

    void VulkanCommandList::Close()
    {
        m_CurrentCommandBuffer->commandBuffer.endRendering();

        m_Context->TransitionImage(m_CurrentCommandBuffer->commandBuffer,
            m_Context->GetSwapchainImages().at(m_Context->GetSwapchainIndex()),
            vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR);

        m_CurrentCommandBuffer->commandBuffer.end();

        m_Context->GetQueue(m_Desc.type)->Submit(this,
            m_Context->GetFrameData().presentCompleteSemaphore, m_Context->GetRenderFinishedSemaphore(),
            m_Context->GetFrameData().inFlightFence);
    }

    void VulkanCommandList::SetGraphicsState(const GraphicsState& state)
    {
        m_CurrentGraphicsState = state;

        VulkanGraphicsPipeline* pipeline = static_cast<VulkanGraphicsPipeline*>(state.pipeline);
        auto bindingSet = static_cast<VulkanBindingSet*>(m_CurrentGraphicsState.bindingSet.Get());
        m_CurrentCommandBuffer->commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline->GetPipelineLayout(),
            0, 1, &bindingSet->descriptorSet, 0, nullptr);
    }

    void VulkanCommandList::SetComputeState(const ComputeState& state)
    {
        m_CurrentComputeState = state;
    }

    void VulkanCommandList::Draw(const DrawArguments& args)
    {
        LOG_DEBUG("Draw");
		QE_ASSERT(m_CurrentGraphicsState.pipeline);
        auto pipeline = dynamic_cast<VulkanGraphicsPipeline*>(m_CurrentGraphicsState.pipeline);
        m_CurrentCommandBuffer->commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetPipeline());
        m_CurrentCommandBuffer->commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f,
            static_cast<float>(m_Context->GetSwapchainExtent().width),
            static_cast<float>(m_Context->GetSwapchainExtent().height), 0.0f, 1.0f));
        m_CurrentCommandBuffer->commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0),
            m_Context->GetSwapchainExtent()));

        if (m_CurrentGraphicsState.vertexBuffer)
        {
            auto vertexBuffer = static_cast<VulkanBuffer*>(m_CurrentGraphicsState.vertexBuffer.Get());
            vk::Buffer buffer = vertexBuffer->GetBuffer();
            vk::DeviceSize offsets[] = {0};
            m_CurrentCommandBuffer->commandBuffer.bindVertexBuffers(0, 1, &buffer, offsets);
        }

        m_CurrentCommandBuffer->commandBuffer.draw(args.vertexCount, args.instanceCount, 0, 0);
    }

    void VulkanCommandList::DrawIndexed(const DrawArguments& args)
    {
        QE_ASSERT(m_CurrentGraphicsState.pipeline);
        auto pipeline = dynamic_cast<VulkanGraphicsPipeline*>(m_CurrentGraphicsState.pipeline);
        m_CurrentCommandBuffer->commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetPipeline());
        m_CurrentCommandBuffer->commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f,
            static_cast<float>(m_Context->GetSwapchainExtent().width),
            static_cast<float>(m_Context->GetSwapchainExtent().height), 0.0f, 1.0f));
        m_CurrentCommandBuffer->commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0),
            m_Context->GetSwapchainExtent()));

        if (m_CurrentGraphicsState.vertexBuffer)
        {
            //LOG_DEBUG("There is a vertex buffer");
            auto vertexBuffer = static_cast<VulkanBuffer*>(m_CurrentGraphicsState.vertexBuffer.Get());
            vk::Buffer buffer = vertexBuffer->GetBuffer();
            vk::DeviceSize offsets[] = {0};
            m_CurrentCommandBuffer->commandBuffer.bindVertexBuffers(0, 1, &buffer, offsets);
        }

        if (m_CurrentGraphicsState.indexBuffer)
        {
            //LOG_DEBUG("There is an index buffer");
            auto indexBuffer = static_cast<VulkanBuffer*>(m_CurrentGraphicsState.indexBuffer.Get());
            vk::Buffer buffer = indexBuffer->GetBuffer();
            vk::DeviceSize offsets[] = {0};
            // TODO: update the indextype to abstract it
            m_CurrentCommandBuffer->commandBuffer.bindIndexBuffer(buffer, offsets[0], vk::IndexType::eUint16);
        }

        m_CurrentCommandBuffer->commandBuffer.drawIndexed(m_CurrentGraphicsState.indexBuffer->GetDesc().size, args.instanceCount, 0, 0, 0);
        //LOG_DEBUG("I did the draw command");
    }

    void VulkanCommandList::DispatchCompute(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ)
    {
    }

    void VulkanCommandList::ClearState()
    {
        m_CurrentGraphicsState = GraphicsState{};
        m_CurrentComputeState = ComputeState{};
    }
}
