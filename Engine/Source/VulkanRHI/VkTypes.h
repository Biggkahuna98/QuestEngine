#pragma once

#include <vulkan/vulkan.h>
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