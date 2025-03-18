#pragma once

#include <string>
#include <cstdint>
#include <vector>
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

	// Vulkan Create Info structs
	VkDebugUtilsMessengerCreateInfoEXT BuildDebugMessengerCreateInfo();

	// Vulkan initialization functions
	void CreateInstance(VkInstance* instance);
	void CreateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT* createInfo, VkDebugUtilsMessengerEXT* messenger, VkInstance* instance);

	// Extra helpers
	uint32_t GetVulkanExtensionCount();
	std::vector<VkExtensionProperties> GetSupportedExtensions();

	uint32_t GetValidationLayerCount();
	std::vector<VkLayerProperties> GetValidationLayerProps();
	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();

	void DestroyDebugMessenger(VkInstance* instance, VkDebugUtilsMessengerEXT* messenger, const VkAllocationCallbacks* allocator = nullptr);
}