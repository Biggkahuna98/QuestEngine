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

	// Vulkan initialization functions - use these in order
	void CreateInstance(VkInstance* instance);
	void CreateDebugMessenger(VkInstance* instance, VkDebugUtilsMessengerCreateInfoEXT* createInfo, VkDebugUtilsMessengerEXT* messenger);
	void PickPhysicalDevice(VkInstance* instance, VkSurfaceKHR* surface, VkPhysicalDevice* physicalDevice);
	void CreateLogicalDevice(VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface, VkQueue* graphicsQueue, VkQueue* presentQueue, VkDevice* device);
	void CreateSwapchain(VkPhysicalDevice* physicalDevice, VkDevice* device, VkSurfaceKHR* surface, VkExtent2D* windowSize, 
		std::vector<VkImage>* swapchainImages, std::vector<VkImageView>* swapchainImageViews, VkFormat* swapchainImageFormat,
		VkExtent2D* swapchainExtent, VkSwapchainKHR* swapchain);
	void CreateSwapchainImageViews(VkDevice* device, std::vector<VkImage>* swapchainImages, VkFormat swapchainImageFormat, std::vector<VkImageView>* swapchainImageViews);
	void CreateGraphicsPipeline(VkDevice* device);

	// Extra helpers
	uint32_t GetVulkanExtensionCount();
	std::vector<VkExtensionProperties> GetSupportedExtensions();

	uint32_t GetValidationLayerCount();
	std::vector<VkLayerProperties> GetValidationLayerProps();
	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();

	void DestroyDebugMessenger(VkInstance* instance, VkDebugUtilsMessengerEXT* messenger, const VkAllocationCallbacks* allocator = nullptr);

	std::string GetPhysicalDeviceName(VkPhysicalDevice physicalDevice);
	void LogPhysicalDeviceProperties(VkPhysicalDevice physicalDevice);

	VkShaderModule CreateShaderModule(VkDevice* device, const std::vector<char>& shaderCode);
}