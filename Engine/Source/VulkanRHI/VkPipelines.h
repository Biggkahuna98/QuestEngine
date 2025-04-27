#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace QE
{
	class PipelineBuilder
	{
	public:
		std::vector<VkPipelineShaderStageCreateInfo> ShaderStages;

		VkPipelineInputAssemblyStateCreateInfo InputAssembly;
		VkPipelineRasterizationStateCreateInfo Rasterizer;
		VkPipelineColorBlendAttachmentState ColorBlendAttachment;
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
		void SetColorAttachmentFormat(VkFormat format);
		void SetDepthFormat(VkFormat format);
		void DisableDepthTest();
	};
}