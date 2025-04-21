#include "VkGraphicsDevice.h"
#include "Core/Log.h"

#include "VkInit.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
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
		LOG_DEBUG_TAG("VkGraphicsDevice", "{} Vulkan Instance extensions supported", VkInit::GetVulkanExtensionCount());
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
		m_Device = VkInit::CreateLogicalDevice(m_PhysicalDevice, m_Surface, &m_GraphicsQueue, &m_PresentQueue);
		LOG_DEBUG_TAG("VkGraphicsDevice", "Vulkan physical device created");

		// Set the queue family indices
		m_QueueFamilyIndices = VkInit::FindQueueFamilies(m_PhysicalDevice, m_Surface);

		// Swapchain
		CreateSwapchain(m_WindowExtent);

		// Command pools and buffers
		InitializeFrameData();
		LOG_DEBUG_TAG("VkGraphicsDevice", "Vulkan command pools and buffers created");

		// Graphics pipeline
		//VkInit::CreateGraphicsPipeline(m_Device, &m_PipelineLayout);

		// VMA Allocator
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = m_PhysicalDevice;
		allocatorInfo.device = m_Device;
		allocatorInfo.instance = m_Instance;
		allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		vmaCreateAllocator(&allocatorInfo, &m_Allocator);

		m_CleanupQueue.PushFunction([&]() {
			vmaDestroyAllocator(m_Allocator);
		});
	}

	VkGraphicsDevice::~VkGraphicsDevice()
	{
		WaitForDeviceIdle();
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkFreeCommandBuffers(m_Device, m_FrameData[i].CommandPool, 1, &m_FrameData[i].CommandBuffer);
			vkDestroyCommandPool(m_Device, m_FrameData[i].CommandPool, nullptr);

			vkDestroyFence(m_Device, m_FrameData[i].RenderFence, nullptr);
			vkDestroySemaphore(m_Device, m_FrameData[i].RenderSemaphore, nullptr);
			vkDestroySemaphore(m_Device, m_FrameData[i].SwapchainSemaphore, nullptr);
		}

		// Flush global lifetime deletion queue
		m_CleanupQueue.Flush();

		// Cleanup all resources
		//vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);

		DestroySwapchain();

		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyDevice(m_Device, nullptr);

		if (VkInit::s_EnableValidationLayers)
			VkInit::DestroyDebugMessenger(&m_Instance, &m_DebugMessenger);
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VkGraphicsDevice::BeginFrame()
	{
		LOG_DEBUG_TAG("VkGraphicsDevice", "Beginning frame: {0}", m_CurrentFrameNumber);
		// Wait for the previous frame to finish
		vkWaitForFences(m_Device, 1, &GetCurrentFrameData().RenderFence, VK_TRUE, UINT64_MAX);
		vkResetFences(m_Device, 1, &GetCurrentFrameData().RenderFence);

		// See if there is a better place later
		GetCurrentFrameData().CleanupQueue.Flush();

		// Request the image from the swapchain
		vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, GetCurrentFrameData().SwapchainSemaphore, VK_NULL_HANDLE, &m_CurrentSwapchainImageIndex);

		// Reset command buffer
		vkResetCommandBuffer(GetCurrentFrameData().CommandBuffer, 0);

		// Begin the buffer for recording
		VkCommandBufferBeginInfo beginInfo = VkInit::BuildCommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		if (vkBeginCommandBuffer(GetCurrentFrameData().CommandBuffer, &beginInfo) != VK_SUCCESS)
		{
			LOG_ERROR_TAG("VkGraphicsDevice", "Failed to begin command buffer recording");
		}
	}

	void VkGraphicsDevice::EndFrame()
	{
		LOG_DEBUG_TAG("VkGraphicsDevice", "Ending frame: {0}", m_CurrentFrameNumber);
		// Come back to the last 2 args
		VkInit::TransitionImage(GetCurrentFrameData().CommandBuffer, m_SwapchainImages[m_CurrentSwapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		// Set clear color
		VkClearColorValue clearColor;
		float flash = std::abs(std::sin(m_CurrentFrameNumber / 360.f));
		clearColor = { { flash/2, flash*3, flash, 1.0f } };

		VkImageSubresourceRange clearRange = VkInit::GetImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

		// Clear image
		vkCmdClearColorImage(GetCurrentFrameData().CommandBuffer, m_SwapchainImages[m_CurrentSwapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1, &clearRange);

		// Make swapchain image into presentable state
		VkInit::TransitionImage(GetCurrentFrameData().CommandBuffer, m_SwapchainImages[m_CurrentSwapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		// End command buffer recording
		if (vkEndCommandBuffer(GetCurrentFrameData().CommandBuffer) != VK_SUCCESS)
		{
			LOG_ERROR_TAG("VkGraphicsDevice", "Failed to end command buffer recording");
		}

		// Submit command buffer to the graphics queue
		VkCommandBufferSubmitInfo cmdSubmitInfo = VkInit::BuildCommandBufferSubmitInfo(GetCurrentFrameData().CommandBuffer);

		VkSemaphoreSubmitInfo waitInfo = VkInit::BuildSemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,GetCurrentFrameData().SwapchainSemaphore);
		VkSemaphoreSubmitInfo signalInfo = VkInit::BuildSemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, GetCurrentFrameData().RenderSemaphore);	
	
		VkSubmitInfo2 submitInfo = VkInit::BuildSubmitInfo2(&cmdSubmitInfo, &signalInfo, &waitInfo);
		if (vkQueueSubmit2(m_GraphicsQueue, 1, &submitInfo, GetCurrentFrameData().RenderFence) != VK_SUCCESS)
		{
			LOG_ERROR_TAG("VkGraphicsDevice", "Failed to submit command buffer to the graphics queue");
			throw std::runtime_error("Failed to submit command buffer to the graphics queue");
		}

		LOG_DEBUG_TAG("VkGraphicsDevice", "Command buffer submitted to the graphics queue");
	}

	void VkGraphicsDevice::PresentFrame()
	{
		LOG_DEBUG_TAG("VkGraphicsDevice", "Presenting frame: {0}", m_CurrentFrameNumber);
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.swapchainCount = 1;

		presentInfo.pWaitSemaphores = &GetCurrentFrameData().SwapchainSemaphore;
		presentInfo.waitSemaphoreCount = 1;

		presentInfo.pImageIndices = &m_CurrentSwapchainImageIndex;

		// Change to present queue later
		if (vkQueuePresentKHR(m_GraphicsQueue, &presentInfo) != VK_SUCCESS)
		{
			LOG_ERROR_TAG("VkGraphicsDevice", "Failed to present swapchain image");
		}

		m_CurrentFrameNumber++;
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
			// Command pools and buffers
			m_FrameData[i].CommandPool = VkInit::CreateCommandPool(m_Device, m_QueueFamilyIndices.graphicsFamily.value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
			m_FrameData[i].CommandBuffer = VkInit::CreateCommandBuffer(m_Device, m_FrameData[i].CommandPool);

			// Semaphores
			m_FrameData[i].SwapchainSemaphore = VkInit::CreateSemaphore(m_Device);
			m_FrameData[i].RenderSemaphore = VkInit::CreateSemaphore(m_Device);

			// Fences
			m_FrameData[i].RenderFence = VkInit::CreateFence(m_Device, VK_FENCE_CREATE_SIGNALED_BIT);
		}
	}
}