#pragma once

#include "RHI/GraphicsContext.h"
#include <vulkan/vulkan.h>

namespace QE
{
	class VkRHIContext : public GraphicsContext
	{
	public:
		VkRHIContext(GraphicsDevice* device) : GraphicsContext(device) {};
	private:
		VkCommandPool m_VkCommandPool;
		VkCommandBuffer m_VkCommandBuffer;
	};
}