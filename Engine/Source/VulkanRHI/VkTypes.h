#pragma once

#include "Core/Log.h"
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vma/vk_mem_alloc.h>
#include "glm/glm.hpp"

namespace QE
{
	struct Vertex
	{
		glm::vec3 Position;
		float uv_x;
		glm::vec3 Normal;
		float uv_y;
		glm::vec4 Color;
	};

	struct AllocatedBuffer
	{
		VkBuffer Buffer;
		VmaAllocation Allocation;
		VmaAllocationInfo AllocationInfo;
	};

	struct GPUMeshBuffer
	{
		AllocatedBuffer VertexBuffer;
		AllocatedBuffer IndexBuffer;
		VkDeviceAddress VertexBufferAddress;
	};

	struct GPUDrawPushConstants
	{
		glm::mat4 WorldMatrix;
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
	
}

#define VK_CHECK(x)                                                                      \
	do {                                                                                 \
		VkResult err = x;                                                                \
		if (err) {                                                                       \
			 LOG_DEBUG_TAG("Vulkan", "Detected Vulkan error: {}", string_VkResult(err)); \
			abort();                                                                     \
		}                                                                                \
	} while (0)