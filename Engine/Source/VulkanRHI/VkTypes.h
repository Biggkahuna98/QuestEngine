#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include "glm/glm.hpp"
#include <array>
#include "RHI/ResourceTypes.h"
#include "VkInit.h"

namespace QE
{
	struct AllocatedBuffer
	{
		VkBuffer Buffer;
		VkDeviceAddress BufferAddress;
		size_t Size;
		VmaAllocation Allocation;
		VmaAllocationInfo AllocationInfo;
	};

	struct GPUMeshBuffer
	{
		BufferHandle VertexBuffer;
		BufferHandle IndexBuffer;
		VkDeviceAddress VertexBufferAddress;
	};

	struct GPUDrawPushConstants
	{
		ModelViewProjection MVP;
		VkDeviceAddress MeshBufferAddress;
	};

	struct AllocatedImage 
	{
		VkImage Image;
		VkImageView ImageView;
		VmaAllocation Allocation;
		VkExtent3D ImageExtent;
		VkFormat ImageFormat;
	};

	struct VulkanShader
	{
		VkShaderModule ShaderModule;
		VkShaderStageFlagBits ShaderStage;
		std::string Name;
	};

	struct VulkanPipeline
	{
		VkPipeline Pipeline;
		VkPipelineLayout PipelineLayout;
		// Cache the description
		PipelineDescription Description;
		std::vector<ShaderHandle> Shaders; // refactor later maybe, these are the handles associated with the shaders in the pipeline
	};

	struct VulkanDevice
	{
		VkPhysicalDevice PhysicalDevice;
		VkDevice Device;
		VkInit::QueueFamilyIndices QueueFamilyIndices;
		VkQueue GraphicsQueue;
		VkQueue PresentQueue;
	};

	struct VulkanSwapchain
	{
		VkSwapchainKHR Swapchain;
		VkSurfaceKHR Surface;
		VkExtent2D SwapchainExtent;
		VkFormat SwapchainImageFormat;
		std::vector<VkImage> SwapchainImages;
		std::vector<VkImageView> SwapchainImageViews;
		std::vector<VkSemaphore> RenderingFinishedSemaphores;
		uint32_t CurrentSwapchainImageIndex;
	};
	
}