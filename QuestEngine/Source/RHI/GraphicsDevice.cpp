#include "RHI/GraphicsDevice.h"

#include "VulkanRHI/VkGraphicsDevice.h"

namespace QE
{
	GraphicsDevice::GraphicsDevice(const Window& window)
	{
	}

	std::unique_ptr<GraphicsDevice> CreateGraphicsDeviceFactory(const Window& window)
	{
		return std::make_unique<VkGraphicsDevice>(window);
	}
}