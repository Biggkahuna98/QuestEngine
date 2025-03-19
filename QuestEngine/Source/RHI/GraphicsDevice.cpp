#include "RHI/GraphicsDevice.h"

#include "VulkanRHI/VkGraphicsDevice.h"

namespace QE
{
	GraphicsDevice::GraphicsDevice(Window* window)
	{
	}

	std::unique_ptr<GraphicsDevice> CreateGraphicsDeviceFactory(Window* window)
	{
		return std::make_unique<VkGraphicsDevice>(window);
	}
}