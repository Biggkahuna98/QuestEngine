#include "VkGraphicsDevice.h"
#include "Core/Log.h"

#include "VkInit.h"

#include "GLFW/glfw3.h"

#include "VKGraphicsContext.h"

constexpr bool enableValidationLayers = true;

namespace QE
{
	VkGraphicsDevice::VkGraphicsDevice(const Window& window)
		: GraphicsDevice(window)
	{
		// Print extension count
		LOG_TAG(Debug, "VkGraphicsDevice", "{} Vulkan extensions supported", VkInit::GetVulkanExtensionCount());
		// Print supported extensions
		std::vector<VkExtensionProperties> extensions = VkInit::GetSupportedExtensions();
		for (const auto& extension : extensions)
		{
			LOG_TAG(Debug, "VkGraphicsDevice", "\t{}", extension.extensionName);
		}

		// Initialize Vulkan and all needed resources
		InitVulkan(window);
		InitSwapchain();
	}

	VkGraphicsDevice::~VkGraphicsDevice()
	{
		// Cleanup all resources
		//DestroySwapchain();

		//vkDestroySurfaceKHR(m_VkInstance, m_VkSurface, nullptr);
		//vkDestroyDevice(m_VkDevice, nullptr);

		if (VkInit::s_EnableValidationLayers)
			VkInit::DestroyDebugMessenger(&m_VkInstance, &m_VkDebugMessenger);
		vkDestroyInstance(m_VkInstance, nullptr);
		
	}

	void VkGraphicsDevice::BeginFrame()
	{
	}

	void VkGraphicsDevice::EndFrame()
	{
	}

	void VkGraphicsDevice::PresentFrame()
	{
	}

	void VkGraphicsDevice::UpdateWindowSize(uint32_t width, uint32_t height)
	{
		LOG_TAG(Debug, "VkGraphicsDevice", "Updating window size: {0}x{1}", width, height);
		m_VkWindowExtent = { width, height };
		RecreateSwapchain();
	}

	std::unique_ptr<GraphicsContext> VkGraphicsDevice::CreateGraphicsContext()
	{
		return std::make_unique<VkGraphicsContext>(this);
	}

	void VkGraphicsDevice::WaitForDeviceIdle()
	{
		vkDeviceWaitIdle(m_VkDevice);
	}

	void VkGraphicsDevice::InitVulkan(const Window& window)
	{
		// Setup the instance
		VkInit::CreateInstance(&m_VkInstance);
		LOG_TAG(Debug, "VkGraphicsDevice", "Vulkan Instance created");

		// Setup debug messenger
		auto debugMessengerCreateInfo = VkInit::BuildDebugMessengerCreateInfo();
		VkInit::CreateDebugMessenger(&debugMessengerCreateInfo, &m_VkDebugMessenger, &m_VkInstance);

		// Setup the surface
		// Only using GLFW for now, change this later to detect window backend later if needed
		//glfwCreateWindowSurface(m_VkInstance, static_cast<GLFWwindow*>(const_cast<Window&>(window).GetNativeWindow()), nullptr, &m_VkSurface);
		LOG_TAG(Debug, "VkGraphicsDevice", "Vulkan surface created and linked to GLFWwindow");

		// Pick a GPU
		LOG_TAG(Debug, "VkGraphicsDevice", "Vulkan device created");
		LOG_TAG(Debug, "VkGraphicsDevice", "Vulkan physical device created");
	}

	void VkGraphicsDevice::InitSwapchain()
	{
		CreateSwapchain(m_VkWindowExtent.width, m_VkWindowExtent.height);
	}

	void VkGraphicsDevice::RecreateSwapchain()
	{
		LOG_TAG(Debug, "VkGraphicsDevice", "Recreating Vulkan Swapchain");
		WaitForDeviceIdle();

		DestroySwapchain();
		CreateSwapchain(m_VkWindowExtent.width, m_VkWindowExtent.height);
	}

	void VkGraphicsDevice::CreateSwapchain(uint32_t width, uint32_t height)
	{
		LOG_TAG(Debug, "VkGraphicsDevice", "Creating Vulkan swapchain");

		LOG_TAG(Debug, "VkGraphicsDevice", "Vulkan Swapchain and views created");
	}

	void VkGraphicsDevice::DestroySwapchain()
	{
		// Destroy swapchain resources
		for (int i = 0; i < m_VkSwapchainImageViews.size(); i++)
		{
			vkDestroyImageView(m_VkDevice, m_VkSwapchainImageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(m_VkDevice, m_VkSwapchain, nullptr);
	}
}