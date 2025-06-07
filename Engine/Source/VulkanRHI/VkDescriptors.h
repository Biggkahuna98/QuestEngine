#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <span>

namespace QE
{
    struct DescriptorLayoutBuilder
    {

        std::vector<VkDescriptorSetLayoutBinding> Bindings;

        void AddBinding(uint32_t binding, VkDescriptorType type);
        void Clear();
        VkDescriptorSetLayout Build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
    };

    struct DescriptorAllocator
    {

        struct PoolSizeRatio
        {
            VkDescriptorType Type;
            float Ratio;
        };

        VkDescriptorPool Pool;

        void InitPool(VkDevice device, std::uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
        void ClearDescriptors(VkDevice device);
        void DestroyPool(VkDevice device);

        VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout);
    };

    struct DescriptorAllocatorGrowable
    {
        struct PoolSizeRatio
        {
            VkDescriptorType Type;
            float Ratio;
        };

        void Init(VkDevice device, std::uint32_t initialSets, std::span<PoolSizeRatio> poolRatios);
        void ClearPools(VkDevice device);
        void DestroyPools(VkDevice device);

        VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout, void* pNext = nullptr);
    private:
        VkDescriptorPool GetPool(VkDevice device);
        VkDescriptorPool CreatePool(VkDevice device, std::uint32_t setCount, std::span<PoolSizeRatio> poolRatios);

        std::vector<PoolSizeRatio> PoolRatios;
        std::vector<VkDescriptorPool> FullPools;
        std::vector<VkDescriptorPool> ReadyPools;
        std::uint32_t SetsPerPool;
    };


}