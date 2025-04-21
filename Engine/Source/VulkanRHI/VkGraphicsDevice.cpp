#include "VkGraphicsDevice.h"
#include "Core/Log.h"

#include "VkInit.h"

#include "GLFW/glfw3.h"

#include "VKGraphicsContext.h"

namespace QE
{
	VkGraphicsDevice::VkGraphicsDevice(Window* window)
		: GraphicsDevice(window)
	{
		// Set real window size
		int width, height;
		glfwGetFramebufferSize(static_cast<GLFWwindow*>(window->GetNativeWindow()), &width, &height);

		m_WindowExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		// Initialize Vulkan and all needed resources
		// Setup the instance
		m_Instance = VkInit::CreateInstance();
		LOG_DEBUG_TAG("VkGraphicsDevice", "Vulkan Instance created");

		// Print extension count
		LOG_DEBUG_TAG("VkGraphicsDevice", "{} Vulkan extensions supported", VkInit::GetVulkanExtensionCount());
		// Print supported extensions
		std::vector<VkExtensionProperties> extensions = VkInit::GetSupportedExtensions();
		for (const auto& extension : extensions)
		{
			LOG_DEBUG_TAG("VkGraphicsDevice", "\t{}", extension.extensionName);
		}

		// Setup debug messenger
		if (VkInit::s_EnableValidationLayers)
		{
			auto debugMessengerCreateInfo = VkInit::BuildDebugMessengerCreateInfo();
			m_DebugMessenger = VkInit::CreateDebugMessenger(m_Instance, debugMessengerCreateInfo);
		}

		// Setup the surface
		// Only using GLFW for now, change this later to detect window backend later if needed
		glfwCreateWindowSurface(m_Instance, static_cast<GLFWwindow*>(window->GetNativeWindow()), nullptr, &m_Surface);
		LOG_DEBUG_TAG("VkGraphicsDevice", "Vulkan surface created and linked to GLFWwindow");

		// Pick a GPU
		m_PhysicalDevice = VkInit::PickPhysicalDevice(m_Instance, m_Surface);
		LOG_DEBUG_TAG("VkGraphicsDevice", "Vulkan device created");

		// Logical device creation
		m_Device = VkInit::CreateLogicalDevice(m_PhysicalDevice, m_Surface, m_GraphicsQueue, m_PresentQueue);
		LOG_DEBUG_TAG("VkGraphicsDevice", "Vulkan physical device created");

		// Set the queue family indices
		m_QueueFamilyIndices = VkInit::FindQueueFamilies(m_PhysicalDevice, m_Surface);

		// Swapchain
		CreateSwapchain(m_WindowExtent);

		// Command pools and buffers
		InitializeFrameData();
		LOG_DEBUG_TAG("VkGraphicsDevice", "Vulkan command pools and buffers created");

		// Graphics pipeline
		VkInit::CreateGraphicsPipeline(m_Device, &m_PipelineLayout);
	}

	VkGraphicsDevice::~VkGraphicsDevice()
	{
		WaitForDeviceIdle();
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkFreeCommandBuffers(m_Device, m_FrameData[i].CommandPool, 1, &m_FrameData[i].CommandBuffer);
			vkDestroyCommandPool(m_Device, m_FrameData[i].CommandPool, nullptr);
		}

		// Cleanup all resources
		vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);

		DestroySwapchain();

		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyDevice(m_Device, nullptr);

		if (VkInit::s_EnableValidationLayers)
			VkInit::DestroyDebugMessenger(&m_Instance, &m_DebugMessenger);
		vkDestroyInstance(m_Instance, nullptr);
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
		LOG_DEBUG_TAG("VkGraphicsDevice", "Updating window size: {0}x{1}", width, height);
		m_WindowExtent = { width, height };
		RecreateSwapchain();
	}

	std::unique_ptr<GraphicsContext> VkGraphicsDevice::CreateGraphicsContext()
	{
		return std::make_unique<VkGraphicsContext>(this);
	}

	void VkGraphicsDevice::WaitForDeviceIdle()
	{
		vkDeviceWaitIdle(m_Device);
	}

	FrameData& VkGraphicsDevice::GetCurrentFrameData()
	{
		return m_FrameData[m_CurrentFrameNumber % MAX_FRAMES_IN_FLIGHT];
	}

	// PRIVATE FUNCTIONS
	void VkGraphicsDevice::CreateSwapchain(VkExtent2D windowExtent)
	{
		LOG_DEBUG_TAG("VkGraphicsDevice", "Creating Vulkan swapchain");
		m_Swapchain = VkInit::CreateSwapchain(m_PhysicalDevice, m_Device, m_Surface, windowExtent, &m_SwapchainImages, &m_SwapchainImageFormat, &m_SwapchainExtent);
		VkInit::CreateSwapchainImageViews(m_Device, &m_SwapchainImages, m_SwapchainImageFormat, &m_SwapchainImageViews);
		LOG_DEBUG_TAG("VkGraphicsDevice", "Vulkan Swapchain and views created");
	}

	void VkGraphicsDevice::RecreateSwapchain()
	{
		LOG_DEBUG_TAG("VkGraphicsDevice", "Recreating Vulkan Swapchain");
		WaitForDeviceIdle();

		DestroySwapchain();
		CreateSwapchain(m_WindowExtent);
	}

	void VkGraphicsDevice::DestroySwapchain()
	{
		// Destroy swapchain resources
		for (int i = 0; i < m_SwapchainImageViews.size(); i++)
		{
			vkDestroyImageView(m_Device, m_SwapchainImageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
	}

	void VkGraphicsDevice::InitializeFrameData()
	{
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_FrameData[i].CommandPool = VkInit::CreateCommandPool(m_Device, m_QueueFamilyIndices.graphicsFamily.value());
			m_FrameData[i].CommandBuffer = VkInit::CreateCommandBuffer(m_Device, m_FrameData[i].CommandPool);
		}
	}
}