#pragma once

#include "VkRHIContext.h"

#include <vulkan/vulkan.h>

namespace QE
{
	class VkGraphicsContext : public VkRHIContext
	{
	public:
		VkGraphicsContext(GraphicsDevice* device);
		~VkGraphicsContext() override;
	private:

	};
}