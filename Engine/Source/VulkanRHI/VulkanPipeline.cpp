#include "VulkanPipeline.h"

#include "VulkanContext.h"
#include "VulkanShader.h"

namespace qrhi::vulkan
{
    // Graphics pipeline
    VulkanGraphicsPipeline::VulkanGraphicsPipeline(GraphicsPipelineDesc desc, const VulkanContext* context)
        : m_Context(context), m_Desc(desc)
    {
        // Vertex input
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};

        // Input assembly
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
        inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;

        vk::Viewport{ 0.0f, 0.0f, static_cast<float>(m_Context->GetSwapchainExtent().width),
            static_cast<float>(m_Context->GetSwapchainExtent().height), 0.0f, 1.0f};
        vk::Rect2D scissor{ vk::Offset2D{ 0, 0 }, m_Context->GetSwapchainExtent()};

        auto dynStates = m_Context->GetDynamicStates();
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo = {};
        dynamicStateInfo.dynamicStateCount = dynStates.size();
        dynamicStateInfo.pDynamicStates = dynStates.data();

        vk::PipelineViewportStateCreateInfo viewportStateInfo = {};
        viewportStateInfo.viewportCount = 1;
        viewportStateInfo.scissorCount = 1;

        vk::PipelineRasterizationStateCreateInfo rasterizer {
            .depthClampEnable = vk::False,
            .rasterizerDiscardEnable = vk::False,
            .polygonMode = vk::PolygonMode::eFill,
            .cullMode = vk::CullModeFlagBits::eBack,
            .frontFace = vk::FrontFace::eClockwise,
            .depthBiasEnable = vk::False,
            .depthBiasSlopeFactor = 1.0f,
            .lineWidth = 1.0f
        };

        vk::PipelineMultisampleStateCreateInfo multisampling {
            .rasterizationSamples = vk::SampleCountFlagBits::e1,
            .sampleShadingEnable = vk::False
        };

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{
            .blendEnable    = vk::False,
            .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
        };

        colorBlendAttachment.blendEnable = vk::True;
        colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
        colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
        colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
        colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

        vk::PipelineColorBlendStateCreateInfo colorBlending {
            .logicOpEnable = vk::False,
            .logicOp =  vk::LogicOp::eCopy,
            .attachmentCount = 1,
            .pAttachments =  &colorBlendAttachment
        };

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo {
            .setLayoutCount = 0,
            .pushConstantRangeCount = 0
        };

        // Create the layout
        m_PipelineLayout = m_Context->GetDevice().createPipelineLayout(pipelineLayoutInfo);

        auto swapchainImgFormat = m_Context->GetSwapchainImageFormat();
        vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo {
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &swapchainImgFormat
        };

        auto shaderModule = dynamic_cast<VulkanShader*>(m_Desc.vertexShader.Get())->GetShaderModule();
        vk::PipelineShaderStageCreateInfo vertShaderStageInfo {
            .stage = vk::ShaderStageFlagBits::eVertex,
            .module = shaderModule,
            .pName = "vertMain"
        };

        vk::PipelineShaderStageCreateInfo fragShaderStageInfo {
            .stage = vk::ShaderStageFlagBits::eFragment,
            .module = shaderModule,
            .pName = "fragMain"
        };

        vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        vk::GraphicsPipelineCreateInfo pipelineInfo {
            .pNext = &pipelineRenderingCreateInfo,
            .stageCount = 2,
            .pStages = shaderStages,
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssemblyInfo,
            .pViewportState = &viewportStateInfo,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicStateInfo,
            .layout = m_PipelineLayout,
            .renderPass = nullptr };

        // Finally create the pipeline
        auto pipeline = m_Context->GetDevice().createGraphicsPipeline(nullptr, pipelineInfo);
        m_Pipeline = pipeline.value;
    }

    VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
    {
        m_Context->GetDevice().destroyPipeline(m_Pipeline);
    }

    Quest::OpaqueObject VulkanGraphicsPipeline::GetNativeType()
    {
        Quest::OpaqueObject obj{};
        obj.pointer = static_cast<void*>(this);
        return obj;
    }

    void VulkanGraphicsPipeline::SetDebugName(const std::string& name)
    {
        m_Context->SetDebugName(m_Pipeline, vk::ObjectType::ePipeline, name);
    }
    
    // Compute pipeline
    VulkanComputePipeline::VulkanComputePipeline(ComputePipelineDesc desc, const VulkanContext* context)
        : m_Context(context), m_Desc(desc)
    {
    }

    VulkanComputePipeline::~VulkanComputePipeline()
    {
    }

    Quest::OpaqueObject VulkanComputePipeline::GetNativeType()
    {
        Quest::OpaqueObject obj{};
        obj.pointer = static_cast<void*>(this);
        return obj;
    }

    void VulkanComputePipeline::SetDebugName(const std::string& name)
    {
        m_Context->SetDebugName(m_Pipeline, vk::ObjectType::ePipeline, name);
    }
}
