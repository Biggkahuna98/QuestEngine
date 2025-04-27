#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <optional>
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

// Initializer helper functions for Vulkan to abstract away the tedium
namespace VkInit
{
#ifdef QE_DEBUG_MODE
	static constexpr bool s_EnableValidationLayers = true;
#else
	static constexpr bool s_EnableValidationLayers = false;
#endif
#ifdef QE_PLATFORM_WINDOWS
#endif
	// Structs
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool IsComplete()
		{
			return graphicsFamily.has_value() &&
				presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	// Vulkan Create Info structs
	VkDebugUtilsMessengerCreateInfoEXT BuildDebugMessengerCreateInfo();
	VkCommandPoolCreateInfo BuildCommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
	VkCommandBufferAllocateInfo BuildCommandBufferAllocateInfo(VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY, uint32_t commandBufferCount = 1);
	VkFenceCreateInfo BuildFenceCreateInfo(VkFenceCreateFlags flags = 0);
	VkSemaphoreCreateInfo BuildSemaphoreCreateInfo(VkSemaphoreCreateFlags flags = 0);
	VkSemaphoreSubmitInfo BuildSemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);
	VkCommandBufferBeginInfo BuildCommandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0);
	VkCommandBufferSubmitInfo BuildCommandBufferSubmitInfo(VkCommandBuffer commandBuffer);
	VkSubmitInfo2 BuildSubmitInfo2(VkCommandBufferSubmitInfo* cmdSubmitInfo, VkSemaphoreSubmitInfo* signalSemaphoreInfo, VkSemaphoreSubmitInfo* waitSemaphoreInfo);
	VkImageCreateInfo BuildImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
	VkImageViewCreateInfo BuildImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);
	VkRenderingAttachmentInfo BuildRenderingAttachmentInfo(VkImageView view, VkClearValue* clear, VkImageLayout layout);
	VkRenderingInfo BuildRenderingInfo(VkExtent2D renderExtent, VkRenderingAttachmentInfo* colorAttachment, VkRenderingAttachmentInfo* depthAttachment);

	// Vulkan initialization functions - use these in order
	VkInstance CreateInstance();
	VkDebugUtilsMessengerEXT CreateDebugMessenger(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT createInfo);
	VkPhysicalDevice PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
	VkDevice CreateLogicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkQueue* graphicsQueue, VkQueue* presentQueue);
	VkSwapchainKHR CreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, VkExtent2D windowSize, 
		std::vector<VkImage>* swapchainImages, VkFormat* swapchainImageFormat, VkExtent2D* swapchainExtent);
	void CreateSwapchainImageViews(VkDevice device, std::vector<VkImage>* swapchainImages, VkFormat swapchainImageFormat, std::vector<VkImageView>* swapchainImageViews);
	void CreateGraphicsPipeline(VkDevice device, VkPipelineLayout* pipelineLayout);

	VkCommandPool CreateCommandPool(VkDevice device, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
	VkCommandBuffer CreateCommandBuffer(VkDevice device, VkCommandPool commandPool);

	VkFence CreateFence(VkDevice device, VkFenceCreateFlags flags = 0);
	VkSemaphore CreateSemaphore(VkDevice device, VkSemaphoreCreateFlags flags = 0);

	// Extra helpers
	uint32_t GetVulkanExtensionCount();
	std::vector<VkExtensionProperties> GetSupportedExtensions();

	uint32_t GetValidationLayerCount();
	std::vector<VkLayerProperties> GetValidationLayerProps();
	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();

	void DestroyDebugMessenger(VkInstance* instance, VkDebugUtilsMessengerEXT* messenger, const VkAllocationCallbacks* allocator = nullptr);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

	std::string GetPhysicalDeviceName(VkPhysicalDevice physicalDevice);
	void LogPhysicalDeviceProperties(VkPhysicalDevice physicalDevice);

	VkImageSubresourceRange GetImageSubresourceRange(VkImageAspectFlags aspectMask);
	
	// Next 3 are generally more helpers than initialization functions
	void TransitionImage(VkCommandBuffer cmdBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
	void CopyImageToImage(VkCommandBuffer cmd, VkImage source, VkImage destination,VkExtent2D srcSize, VkExtent2D dstSize);

	VkShaderModule CreateShaderModule(VkDevice device, const std::string_view& filename);
	VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& shaderCode);
}