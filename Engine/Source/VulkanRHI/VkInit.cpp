#include "VkInit.h"
#include "Core/Log.h"

#include <GLFW/glfw3.h>
#include <set>
#include <algorithm>
#include <fstream>
#include <filesystem>

namespace VkInit
{
	// Constants
	static const std::vector<const char*> s_ValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	static const std::vector<const char*> s_DeviceExtensions = {
		// 1.3 extensions
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,

		// 1.2 extensions
		VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
		VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
	};

	// Debug messenger
	static VKAPI_ATTR VkBool32 VKAPI_CALL s_VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		(void)pUserData; // Mark pUserData as unused
		LOG_ERROR("[{}: {}] {}", string_VkDebugUtilsMessageSeverityFlagBitsEXT(messageSeverity), string_VkDebugUtilsMessageTypeFlagsEXT(messageType), pCallbackData->pMessage);
	
		return VK_FALSE;
	}

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

	static bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(s_DeviceExtensions.begin(), s_DeviceExtensions.end());
		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		for (const auto& extension : requiredExtensions)
		{
			LOG_ERROR_TAG("VkInit", "Missing device extension: {}", extension);
		}
		return requiredExtensions.empty();
	}

	static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	static bool IsPhysicalDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		QueueFamilyIndices indices = FindQueueFamilies(device, surface);

		bool extensionsSupported = CheckDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}
		
		return indices.IsComplete() && extensionsSupported && swapChainAdequate;
	}

	static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				LOG_DEBUG("Available format!");
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D actualExtent)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}

	static std::vector<char> ReadShaderFile(const std::string& filename)
	{
		std::string filename_ = QE_RESOURCES_FOLDER;
		filename_.append("ShaderCache/").append(filename);
		LOG_DEBUG_TAG("VkInit", "Shader - {}: path: {}", filename, filename_);

		if (!std::filesystem::exists(filename_))
		{
			LOG_DEBUG_TAG("VkInit", "File does not exist: {}", filename_);
			return {};
		}

		std::ifstream file(filename_, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open shader file");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	// Create info functions
	VkDebugUtilsMessengerCreateInfoEXT BuildDebugMessengerCreateInfo()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = s_VkDebugCallback;

		return createInfo;
	}

	VkCommandPoolCreateInfo BuildCommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
	{
		VkCommandPoolCreateInfo commandPoolInfo = {};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.pNext = nullptr;
		commandPoolInfo.queueFamilyIndex = queueFamilyIndex;
		commandPoolInfo.flags = flags;

		return commandPoolInfo;
	}

	VkCommandBufferAllocateInfo BuildCommandBufferAllocateInfo(VkCommandPool commandPool, VkCommandBufferLevel level, uint32_t commandBufferCount)
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.level = level;
		commandBufferAllocateInfo.commandBufferCount = commandBufferCount;

		return commandBufferAllocateInfo;
	}
	
	VkFenceCreateInfo BuildFenceCreateInfo(VkFenceCreateFlags flags)
	{
		VkFenceCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = flags;

		return info;
	}

	VkSemaphoreCreateInfo BuildSemaphoreCreateInfo(VkSemaphoreCreateFlags flags)
	{
		VkSemaphoreCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = flags;

		return info;
	}

	VkSemaphoreSubmitInfo BuildSemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore)
	{
		VkSemaphoreSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		info.pNext = nullptr;
		info.stageMask = stageMask;
		info.semaphore = semaphore;
		info.deviceIndex = 0;
		info.value = 1;

		return info;
	}

	VkCommandBufferBeginInfo BuildCommandBufferBeginInfo(VkCommandBufferUsageFlags flags)
	{
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.pNext = nullptr;
		info.flags = flags;

		return info;
	}

	VkCommandBufferSubmitInfo BuildCommandBufferSubmitInfo(VkCommandBuffer commandBuffer)
	{
		VkCommandBufferSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
		info.pNext = nullptr;
		info.commandBuffer = commandBuffer;
		info.deviceMask = 0;

		return info;
	}

	VkSubmitInfo2 BuildSubmitInfo2(VkCommandBufferSubmitInfo* cmdSubmitInfo, VkSemaphoreSubmitInfo* signalSemaphoreInfo, VkSemaphoreSubmitInfo* waitSemaphoreInfo)
	{
		VkSubmitInfo2 info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		info.pNext = nullptr;
	
		info.waitSemaphoreInfoCount = waitSemaphoreInfo == nullptr ? 0 : 1;
		info.pWaitSemaphoreInfos = waitSemaphoreInfo;
	
		info.signalSemaphoreInfoCount = signalSemaphoreInfo == nullptr ? 0 : 1;
		info.pSignalSemaphoreInfos = signalSemaphoreInfo;
	
		info.commandBufferInfoCount = 1;
		info.pCommandBufferInfos = cmdSubmitInfo;
	
		return info;
	}

	VkImageCreateInfo BuildImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent)
	{
		VkImageCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		info.pNext = nullptr;

		info.imageType = VK_IMAGE_TYPE_2D;

		info.format = format;
		info.extent = extent;

		info.mipLevels = 1;
		info.arrayLayers = 1;

		//for MSAA. we will not be using it by default, so default it to 1 sample per pixel.
		info.samples = VK_SAMPLE_COUNT_1_BIT;

		//optimal tiling, which means the image is stored on the best gpu format
		info.tiling = VK_IMAGE_TILING_OPTIMAL;
		info.usage = usageFlags;

		return info;
	}

	VkImageViewCreateInfo BuildImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags)
	{
		// build a image-view for the depth image to use for rendering
		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.pNext = nullptr;

		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.image = image;
		info.format = format;
		info.subresourceRange.baseMipLevel = 0;
		info.subresourceRange.levelCount = 1;
		info.subresourceRange.baseArrayLayer = 0;
		info.subresourceRange.layerCount = 1;
		info.subresourceRange.aspectMask = aspectFlags;

		return info;
	}

	VkRenderingAttachmentInfo BuildRenderingAttachmentInfo(VkImageView view, VkClearValue* clear, VkImageLayout layout)
	{
		VkRenderingAttachmentInfo colorAttachment{};
		colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachment.pNext = nullptr;

		colorAttachment.imageView = view;
		colorAttachment.imageLayout = layout;
		colorAttachment.loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		if (clear)
		{
			colorAttachment.clearValue = *clear;
		}

		return colorAttachment;
	}

	VkRenderingInfo BuildRenderingInfo(VkExtent2D renderExtent, VkRenderingAttachmentInfo* colorAttachment, VkRenderingAttachmentInfo* depthAttachment)
	{
		VkRenderingInfo renderInfo{};
		renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderInfo.pNext = nullptr;

		renderInfo.renderArea = VkRect2D{ VkOffset2D { 0, 0 }, renderExtent };
		renderInfo.layerCount = 1;
		renderInfo.colorAttachmentCount = 1;
		renderInfo.pColorAttachments = colorAttachment;
		renderInfo.pDepthAttachment = depthAttachment;
		renderInfo.pStencilAttachment = nullptr;

		return renderInfo;
	}

	VkPipelineLayoutCreateInfo BuildPipelineCreateInfo()
	{
		VkPipelineLayoutCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info.pNext = nullptr;

		// empty defaults
		info.flags = 0;
		info.setLayoutCount = 0;
		info.pSetLayouts = nullptr;
		info.pushConstantRangeCount = 0;
		info.pPushConstantRanges = nullptr;
		return info;
	}

	VkPipelineShaderStageCreateInfo BuildShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shaderModule, const char* entryPoint)
	{
		VkPipelineShaderStageCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		info.pNext = nullptr;

		// shader stage
		info.stage = stage;
		// module containing the code for this shader stage
		info.module = shaderModule;
		// the entry point of the shader
		info.pName = entryPoint;
		return info;
	}

	// Vulkan object builders
	VkInstance CreateInstance()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Quest Application";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Quest Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;

		auto extensions = GetRequiredExtensions();
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (s_EnableValidationLayers)
		{
			instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
			instanceCreateInfo.ppEnabledLayerNames = s_ValidationLayers.data();

			debugCreateInfo = BuildDebugMessengerCreateInfo();
			instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			instanceCreateInfo.enabledLayerCount = 0;
			instanceCreateInfo.pNext = nullptr;
		}

		// Check validation layer support
		if (s_EnableValidationLayers && !CheckValidationLayerSupport())
			throw std::runtime_error("Validation layers requested, none available");

		VkInstance instance;
		VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);

		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to create Vulkan Instance");
		
		return instance;
	}

	VkDebugUtilsMessengerEXT CreateDebugMessenger(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT createInfo)
	{
		VkDebugUtilsMessengerEXT messenger;

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &messenger) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to set up debug messenger");
		}

		return messenger;
	}

	VkPhysicalDevice PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		LOG_DEBUG("Physical Devices detected: {}", deviceCount);

		if (deviceCount == 0)
		{
			throw std::runtime_error("Failed to find GPUs with Vulkan support");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		bool isDeviceFound = false;
		for (const auto& device : devices)
		{
			LogPhysicalDeviceProperties(device);

			if (!isDeviceFound && IsPhysicalDeviceSuitable(device, surface))
			{
				physicalDevice = device;
				isDeviceFound = true;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE)
		{
			throw std::runtime_error("Failed to find a suitable GPU");
		}

		LOG_DEBUG("Device Chosen: {}", GetPhysicalDeviceName((physicalDevice)));

		return physicalDevice;
	}

	VkDevice CreateLogicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkQueue* graphicsQueue, VkQueue* presentQueue)
	{
		QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {
			indices.graphicsFamily.value(),
			indices.presentFamily.value()
		};

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(s_DeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = s_DeviceExtensions.data();

		// Synchronization 2 extension
		VkPhysicalDeviceSynchronization2Features sync2Features = {};
		sync2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
		sync2Features.pNext = nullptr;
		sync2Features.synchronization2 = VK_TRUE;

		// Buffer Device Address extension
		VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = {};
		bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
		bufferDeviceAddressFeatures.pNext = &sync2Features;
		bufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;

		// Dynamic Rendering extension
		VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures = {};
		dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
		dynamicRenderingFeatures.pNext = &bufferDeviceAddressFeatures;
		dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

		createInfo.pNext = &dynamicRenderingFeatures;
		
		VkDevice device = VK_NULL_HANDLE;

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create logical device");
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, presentQueue);

		return device;
	}

	VkSwapchainKHR CreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, VkExtent2D windowSize, std::vector<VkImage>* swapchainImages, VkFormat* swapchainImageFormat, VkExtent2D* swapchainExtent)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice, surface);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, windowSize);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		// Change image usage back to VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
		//VK_IMAGE_USAGE_TRANSFER_DST_BIT
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VkSwapchainKHR swapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create swap chain");
		}

		vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
		swapchainImages->resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages->data());

		*swapchainImageFormat = surfaceFormat.format;
		*swapchainExtent = extent;

		return swapchain;
	}

	void CreateSwapchainImageViews(VkDevice device, std::vector<VkImage>* swapchainImages, VkFormat swapchainImageFormat, std::vector<VkImageView>* swapchainImageViews)
	{
		swapchainImageViews->resize(swapchainImages->size());

		for (size_t i = 0; i < swapchainImages->size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapchainImages->at(i);
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapchainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;
			if (vkCreateImageView(device, &createInfo, nullptr, &swapchainImageViews->at(i)) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create image views");
			}
		}
	}

	void CreateGraphicsPipeline(VkDevice device, VkFormat swapchainImageFormat, VkPipelineLayout* pipelineLayout)
	{
		auto vertShaderCode = ReadShaderFile("triangle-vert.spv");
		auto fragShaderCode = ReadShaderFile("triangle-frag.spv");

		VkShaderModule vertShaderModule = CreateShaderModule(device, vertShaderCode);
		VkShaderModule fragShaderModule = CreateShaderModule(device, fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		// UPDATE THE COLOR ATTACHMENT FORMAT WITH SWAPCHAIN IMAGE FORMAT
		// https://lesleylai.info/en/vk-khr-dynamic-rendering/
		VkPipelineRenderingCreateInfo RenderingInfo{};
		RenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		RenderingInfo.colorAttachmentCount = 1;
		RenderingInfo.pColorAttachmentFormats = &swapchainImageFormat;

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}

		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
	}

	VkCommandPool CreateCommandPool(VkDevice device, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
	{
		//create a command pool for commands submitted to the graphics queue.
		//we also want the pool to allow for resetting of individual command buffers
		VkCommandPoolCreateInfo commandPoolInfo =  VkInit::BuildCommandPoolCreateInfo(queueFamilyIndex, flags);
		
		VkCommandPool commandPool = VK_NULL_HANDLE;
		if(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &commandPool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create command pool");
		}

		return commandPool;
	}

	VkCommandBuffer CreateCommandBuffer(VkDevice device, VkCommandPool commandPool)
	{
		VkCommandBufferAllocateInfo cmdAllocInfo = VkInit::BuildCommandBufferAllocateInfo(commandPool);

		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		if (vkAllocateCommandBuffers(device, &cmdAllocInfo, &commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffer");
		}

		return commandBuffer;
	}

	VkFence CreateFence(VkDevice device, VkFenceCreateFlags flags)
	{
		VkFenceCreateInfo fenceInfo = VkInit::BuildFenceCreateInfo(flags);

		VkFence fence = VK_NULL_HANDLE;
		if (vkCreateFence(device, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create fence");
		}

		return fence;
	}

	VkSemaphore CreateSemaphore(VkDevice device, VkSemaphoreCreateFlags flags)
	{
		VkSemaphoreCreateInfo semaphoreInfo = VkInit::BuildSemaphoreCreateInfo(flags);

		VkSemaphore semaphore = VK_NULL_HANDLE;
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create semaphore");
		}

		return semaphore;
	}

	// Extra helpers
	uint32_t GetVulkanExtensionCount()
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		return extensionCount;
	}

	std::vector<VkExtensionProperties> GetSupportedExtensions()
	{
		uint32_t extensionCount = GetVulkanExtensionCount();
		std::vector<VkExtensionProperties> extensions(extensionCount);

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		
		return extensions;
	}

	uint32_t GetValidationLayerCount()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		return layerCount;
	}

	std::vector<VkLayerProperties> GetValidationLayerProps()
	{
		uint32_t layerCount = GetValidationLayerCount();
		std::vector<VkLayerProperties> availableLayers(layerCount);

		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		return availableLayers;
		
	}
	bool CheckValidationLayerSupport()
	{
		std::vector<VkLayerProperties> availableLayers = GetValidationLayerProps();

		for (auto* layerName : s_ValidationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
				return false;
		}

		return true;
	}

	std::vector<const char*> GetRequiredExtensions()
	{
		uint32_t glfwExtensionsCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);

		if (s_EnableValidationLayers)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}

	void DestroyDebugMessenger(VkInstance* instance, VkDebugUtilsMessengerEXT* messenger, const VkAllocationCallbacks* allocator)
	{
		DestroyDebugUtilsMessengerEXT(*instance, *messenger, allocator);
	}

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (presentSupport)
			{
				indices.presentFamily = i;
			}

			if (indices.IsComplete())
			{
				break;
			}

			i++;
		}

		return indices;
	}

	std::string GetPhysicalDeviceName(VkPhysicalDevice physicalDevice)
	{
		VkPhysicalDeviceProperties deviceProps;
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProps);

		return deviceProps.deviceName;
	}

	void LogPhysicalDeviceProperties(VkPhysicalDevice physicalDevice)
	{
		VkPhysicalDeviceProperties deviceProps;
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProps);
		
		LOG_DEBUG("Device Name: {}", deviceProps.deviceName);
		LOG_DEBUG("\tAPI Version: {}", deviceProps.apiVersion);
		LOG_DEBUG("\tDriver Version: {}", deviceProps.driverVersion);
		LOG_DEBUG("\tVendor ID: {}", deviceProps.vendorID);
		LOG_DEBUG("\tDevice Type: {}", string_VkPhysicalDeviceType(deviceProps.deviceType));
	}

	VkImageSubresourceRange GetImageSubresourceRange(VkImageAspectFlags aspectMask)
	{
		VkImageSubresourceRange subImage {};
		subImage.aspectMask = aspectMask;
		subImage.baseMipLevel = 0;
		subImage.levelCount = VK_REMAINING_MIP_LEVELS;
		subImage.baseArrayLayer = 0;
		subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

		return subImage;
	}

	void TransitionImage(VkCommandBuffer cmdBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkImageMemoryBarrier2 imageBarrier = {};
		imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		imageBarrier.pNext = nullptr;

		imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
		imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

		imageBarrier.oldLayout = oldLayout;
		imageBarrier.newLayout = newLayout;

		VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		imageBarrier.subresourceRange = GetImageSubresourceRange(aspectMask);
		imageBarrier.image = image;
	
		VkDependencyInfo depInfo {};
		depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		depInfo.pNext = nullptr;
	
		depInfo.imageMemoryBarrierCount = 1;
		depInfo.pImageMemoryBarriers = &imageBarrier;
	
		vkCmdPipelineBarrier2(cmdBuffer, &depInfo);
	}

	void CopyImageToImage(VkCommandBuffer cmd, VkImage source, VkImage destination,VkExtent2D srcSize, VkExtent2D dstSize)
	{
		VkImageBlit2 blitRegion{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

		blitRegion.srcOffsets[1].x = srcSize.width;
		blitRegion.srcOffsets[1].y = srcSize.height;
		blitRegion.srcOffsets[1].z = 1;

		blitRegion.dstOffsets[1].x = dstSize.width;
		blitRegion.dstOffsets[1].y = dstSize.height;
		blitRegion.dstOffsets[1].z = 1;

		blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blitRegion.srcSubresource.baseArrayLayer = 0;
		blitRegion.srcSubresource.layerCount = 1;
		blitRegion.srcSubresource.mipLevel = 0;

		blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blitRegion.dstSubresource.baseArrayLayer = 0;
		blitRegion.dstSubresource.layerCount = 1;
		blitRegion.dstSubresource.mipLevel = 0;

		VkBlitImageInfo2 blitInfo{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
		blitInfo.dstImage = destination;
		blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		blitInfo.srcImage = source;
		blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		blitInfo.filter = VK_FILTER_LINEAR;
		blitInfo.regionCount = 1;
		blitInfo.pRegions = &blitRegion;

		vkCmdBlitImage2(cmd, &blitInfo);
	}

	VkShaderModule CreateShaderModule(VkDevice device, const std::string_view& filename)
	{
		// Get some kind of error checking for empty shader code
		auto shaderCode = ReadShaderFile(filename.data());

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
		

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module");
		}

		return shaderModule;
	}


	VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& shaderCode)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to create shader module");
		}

		return shaderModule;
	}
}