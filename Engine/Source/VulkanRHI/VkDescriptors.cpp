#include "VkDescriptors.h"
#include "VkTypes.h"
#include "VkCommon.h"

namespace QE
{
	void DescriptorLayoutBuilder::AddBinding(uint32_t binding, VkDescriptorType type)
	{
		VkDescriptorSetLayoutBinding newBind{};
		newBind.binding = binding;
		newBind.descriptorCount = 1;
		newBind.descriptorType = type;

		Bindings.push_back(newBind);
	}

	void DescriptorLayoutBuilder::Clear()
	{
		Bindings.clear();
	}

	VkDescriptorSetLayout DescriptorLayoutBuilder::Build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext, VkDescriptorSetLayoutCreateFlags flags)
	{
		for (auto& b : Bindings)
		{
			b.stageFlags |= shaderStages;
		}

		VkDescriptorSetLayoutCreateInfo info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		info.pNext = pNext;

		info.pBindings = Bindings.data();
		info.bindingCount = (uint32_t)Bindings.size();
		info.flags = flags;

		VkDescriptorSetLayout set;
		VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));

		return set;
	}

	void DescriptorAllocator::InitPool(VkDevice device, std::uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;
		for (PoolSizeRatio ratio : poolRatios)
		{
			poolSizes.push_back(VkDescriptorPoolSize{
				.type = ratio.Type,
				.descriptorCount = uint32_t(ratio.Ratio * maxSets)
			});
		}

		VkDescriptorPoolCreateInfo pool_info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		pool_info.flags = 0;
		pool_info.maxSets = maxSets;
		pool_info.poolSizeCount = (uint32_t)poolSizes.size();
		pool_info.pPoolSizes = poolSizes.data();

		vkCreateDescriptorPool(device, &pool_info, nullptr, &Pool);
	}

	void DescriptorAllocator::ClearDescriptors(VkDevice device)
	{
		vkResetDescriptorPool(device, Pool, 0);
	}

	void DescriptorAllocator::DestroyPool(VkDevice device)
	{
		vkDestroyDescriptorPool(device, Pool, nullptr);
	}

	VkDescriptorSet DescriptorAllocator::Allocate(VkDevice device, VkDescriptorSetLayout layout)
	{
		VkDescriptorSetAllocateInfo allocInfo = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.pNext = nullptr;
		allocInfo.descriptorPool = Pool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &layout;

		VkDescriptorSet ds;
		VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &ds));

		return ds;
	}

	void DescriptorAllocatorGrowable::Init(VkDevice device, std::uint32_t initialSets, std::span<PoolSizeRatio> poolRatios)
	{
		PoolRatios.clear();

		for (auto ratio : poolRatios)
			PoolRatios.push_back(ratio);

		VkDescriptorPool newPool = CreatePool(device, initialSets, PoolRatios);

		SetsPerPool = initialSets * 1.5; // grow next allocation

		ReadyPools.push_back(newPool);
	}

	void DescriptorAllocatorGrowable::ClearPools(VkDevice device)
	{
		for (auto pool : ReadyPools)
			vkResetDescriptorPool(device, pool, 0);
		for (auto pool : FullPools)
		{
			vkResetDescriptorPool(device, pool, 0);
			ReadyPools.push_back(pool);
		}
		FullPools.clear();
	}

	void DescriptorAllocatorGrowable::DestroyPools(VkDevice device)
	{
		for (auto pool : ReadyPools)
			vkDestroyDescriptorPool(device, pool, nullptr);
		for (auto pool : FullPools)
			vkDestroyDescriptorPool(device, pool, nullptr);

		ReadyPools.clear();
		FullPools.clear();
	}

	VkDescriptorSet DescriptorAllocatorGrowable::Allocate(VkDevice device, VkDescriptorSetLayout layout, void *pNext)
	{
		VkDescriptorPool poolToUse = GetPool(device);

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.pNext = pNext;
		allocInfo.descriptorPool = poolToUse;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &layout;

		VkDescriptorSet ds;
		VkResult result = vkAllocateDescriptorSets(device, &allocInfo, &ds);

		// Allocation failed, try another pool (if it fails a second time, stuff is too broken and crash)
		if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL)
		{
			FullPools.push_back(poolToUse);
			poolToUse = GetPool(device);
			allocInfo.descriptorPool = poolToUse;

			VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &ds));
		}

		ReadyPools.push_back(poolToUse);

		return ds;
	}

	VkDescriptorPool DescriptorAllocatorGrowable::GetPool(VkDevice device)
	{
		VkDescriptorPool newPool;
		if (ReadyPools.size() != 0)
		{
			newPool = ReadyPools.back();
			ReadyPools.pop_back();
		} else
		{
			// Create a new pool
			newPool = CreatePool(device, SetsPerPool, PoolRatios);

			SetsPerPool = SetsPerPool * 1.5;
			if (SetsPerPool > 4096)
				SetsPerPool = 4096;
		}

		return newPool;
	}

	VkDescriptorPool DescriptorAllocatorGrowable::CreatePool(VkDevice device, std::uint32_t setCount, std::span<PoolSizeRatio> poolRatios)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;
		for (PoolSizeRatio ratio : poolRatios)
		{
			poolSizes.push_back(VkDescriptorPoolSize{
				.type = ratio.Type,
				.descriptorCount = std::uint32_t(ratio.Ratio * setCount)
			});
		}

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = 0;
		pool_info.maxSets = setCount;
		pool_info.poolSizeCount = (std::uint32_t)poolSizes.size();
		pool_info.pPoolSizes = poolSizes.data();

		VkDescriptorPool newPool;
		VK_CHECK(vkCreateDescriptorPool(device, &pool_info, nullptr, &newPool));

		return newPool;
	}


}