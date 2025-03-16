#include "RHI/GraphicsContext.h"

#include "VulkanRHI/VKGraphicsContext.h"

namespace QE
{
	GraphicsContext::GraphicsContext(GraphicsDevice* device)
		: RHIContext(device)
	{
	}

	/*std::unique_ptr<GraphicsContext> CreateGraphicsContextFactory(GraphicsDevice* device)
	{
		return std::make_unique<VkGraphicsContext>(device);
	}*/
}