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

	void DescriptorAllocator::InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
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
}