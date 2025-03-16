#include "VkGraphicsDevice.h"
#include "Core/Log.h"

#include "GLFW/glfw3.h"

constexpr bool enableValidationLayers = true;

namespace QE
{
	VkGraphicsDevice::VkGraphicsDevice(const Window& window)
		: GraphicsDevice(window)
	{
		// Initialize Vulkan and all needed resources
		InitVulkan(window);
		InitSwapchain();
	}

	VkGraphicsDevice::~VkGraphicsDevice()
	{
		// Cleanup all resources
		DestroySwapchain();

		vkDestroySurfaceKHR(m_VkInstance, m_VkSurface, nullptr);
		vkDestroyDevice(m_VkDevice, nullptr);

		vkb::destroy_debug_utils_messenger(m_VkInstance, m_VkDebugMessenger, nullptr);
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

	void VkGraphicsDevice::WaitForDeviceIdle()
	{
		vkDeviceWaitIdle(m_VkDevice);
	}

	void VkGraphicsDevice::InitVulkan(const Window& window)
	{
		// Setup the instance
		vkb::InstanceBuilder builder;
		
		builder.set_app_name("Quest Engine")
			.request_validation_layers(enableValidationLayers)
			.require_api_version(1, 4, 0);

		builder.set_debug_callback(
				[](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
					VkDebugUtilsMessageTypeFlagsEXT messageType,
					const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
					void* pUserData)
				-> VkBool32 {
					auto severity = vkb::to_string_message_severity(messageSeverity);
					auto type = vkb::to_string_message_type(messageType);
					LOG(Debug, "[{}: {}] {}", severity, type, pCallbackData->pMessage);
					return VK_FALSE;
				}
		);

		vkb::Instance vkbInstance = builder.build().value();

		m_VkInstance = vkbInstance.instance;
		m_VkDebugMessenger = vkbInstance.debug_messenger;
		LOG_TAG(Debug, "VkGraphicsDevice", "Vulkan Instance created");

		// Setup the surface
		// Only using GLFW for now, change this later to detect window backend later if needed
		glfwCreateWindowSurface(m_VkInstance, static_cast<GLFWwindow*>(const_cast<Window&>(window).GetNativeWindow()), nullptr, &m_VkSurface);
		LOG_TAG(Debug, "VkGraphicsDevice", "Vulkan surface created and linked to GLFWwindow");

		// Vulkan 1.4 features
		VkPhysicalDeviceVulkan14Features features14{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES };

		// Vulkan 1.3 features
		VkPhysicalDeviceVulkan13Features features13{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
		features13.dynamicRendering = true;
		features13.synchronization2 = true;

		// Vulkan 1.2 features
		VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
		features12.bufferDeviceAddress = true;
		features12.descriptorIndexing = true;

		// Pick a GPU
		vkb::PhysicalDeviceSelector selector{ vkbInstance };
		vkb::PhysicalDevice physicalDevice = selector
			.set_minimum_version(1, 3)
			.set_required_features_14(features14)
			.set_required_features_13(features13)
			.set_required_features_12(features12)
			.set_surface(m_VkSurface)
			.select()
			.value();

		vkb::DeviceBuilder deviceBuilder{ physicalDevice };
		vkb::Device vkbDevice = deviceBuilder.build().value();

		m_VkDevice = vkbDevice.device;
		m_VkPhysicalDevice = physicalDevice.physical_device;
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
		vkb::SwapchainBuilder swapchainBuilder{ m_VkPhysicalDevice, m_VkDevice, m_VkSurface };

		m_VkSwapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

		vkb::Swapchain vkbSwapchain = swapchainBuilder
			//.use_default_format_selection()
			.set_desired_format(VkSurfaceFormatKHR{ .format = m_VkSwapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
			.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
			.set_desired_extent(width, height)
			.build()
			.value();

		m_VkSwapchainExtent = vkbSwapchain.extent;
		m_VkSwapchain = vkbSwapchain.swapchain;
		m_VkSwapchainImages = vkbSwapchain.get_images().value();
		m_VkSwapchainImageViews = vkbSwapchain.get_image_views().value();
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