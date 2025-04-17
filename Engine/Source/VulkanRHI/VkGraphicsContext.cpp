#include "VKGraphicsContext.h"

namespace QE
{
	VkGraphicsContext::VkGraphicsContext(GraphicsDevice* device)
		: VkRHIContext(device)
	{
	}
	VkGraphicsContext::~VkGraphicsContext()
	{
	}
}