#pragma once

#include "VkRHIContext.h"

#include <vulkan/vulkan.h>

namespace QE
{
	class VkGraphicsContext : public VkRHIContext
	{
	public:
		VkGraphicsContext(GraphicsDevice* device);
		virtual ~VkGraphicsContext() override;
	private:
		VkCommandPool m_VkCommandPool;
		VkCommandBuffer m_VkCommandBuffer;

	};
}