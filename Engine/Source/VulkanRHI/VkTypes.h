#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include "glm/glm.hpp"
#include <array>
#include "RHI/ResourceTypes.h"

namespace QE
{
	struct VertexOld
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

	struct VertexBuffer
	{
		AllocatedBuffer Buffer;
		size_t Size;
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