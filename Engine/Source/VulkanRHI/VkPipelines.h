#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>

namespace QE
{
	class PipelineBuilder
	{
	public:
		std::vector<VkPipelineShaderStageCreateInfo> ShaderStages;

		VkPipelineInputAssemblyStateCreateInfo InputAssembly;
		VkPipelineRasterizationStateCreateInfo Rasterizer;
		VkPipelineColorBlendAttachmentState ColorBlendAttachment;
		VkPipelineVertexInputStateCreateInfo VertexInput;
		VkPipelineMultisampleStateCreateInfo Multisampling;
		VkPipelineLayout PipelineLayout;
		VkPipelineDepthStencilStateCreateInfo DepthStencil;
		VkPipelineRenderingCreateInfo RenderingInfo;
		VkFormat ColorAttachmentFormat;

		PipelineBuilder();
		void Clear();

		VkPipeline BuildPipeline(VkDevice device);

		void SetShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
		void SetInputTopology(VkPrimitiveTopology topology);
		void SetPolygonMode(VkPolygonMode mode);
		void SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace);
		void SetMultisamplingMode(); // is disabled for now
		void DisableBlending();
		void UseVertexInput();
		void SetColorAttachmentFormat(VkFormat format);
		void SetDepthFormat(VkFormat format);
		void DisableDepthTest();

	private:
		VkVertexInputBindingDescription m_VertexInputBindingDesc;
		std::array<VkVertexInputAttributeDescription, 2> m_VertexInputAttributeDescs;
	};
}