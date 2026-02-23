#include "GPU/Descriptors/GPUDescriptorPool.hpp"

void GPUDescriptorPool::init(GPUContext* gpuContext)
{
    mGPUContext = gpuContext;

    VkPhysicalDeviceDescriptorIndexingProperties indexingProps{};
    indexingProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;

    VkPhysicalDeviceProperties2 deviceProps{};
    deviceProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    deviceProps.pNext = &indexingProps;

    vkGetPhysicalDeviceProperties2(mGPUContext->vulkanPhysicalDevice->getPhysicalDevice(), &deviceProps);

    // POOL
    constexpr Core::u32 poolTypesCount = 3;
    std::array<VkDescriptorPoolSize, poolTypesCount> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[0].descriptorCount = 200; // TODO: Set value to indexingProps.maxDescriptorSetUpdateAfterBindStorageBuffers;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = 200; // TODO: Set value to indexingProps.maxDescriptorSetUpdateAfterBindUniformBuffers;
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[2].descriptorCount = 4096; // TODO: Set value to indexingProps.maxDescriptorSetUpdateAfterBindSamplers;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    poolInfo.poolSizeCount = poolTypesCount;
    poolInfo.pPoolSizes = poolSizes.data();
    // TODO: select a correct poolInfo.maxSets number, GPUContext::MAX_FRAMES_IN_FLIGHT should be enough for the Pool used for GLOBAL DescriptorSet
    poolInfo.maxSets = GPUContext::MAX_FRAMES_IN_FLIGHT * 100;

    /*
        * Inadequate descriptor pools are a good example of a problem that the validation layers will not catch:
        * As of Vulkan 1.1, vkAllocateDescriptorSets may fail with the error code VK_ERROR_POOL_OUT_OF_MEMORY if the pool is not sufficiently large,
        * but the driver may also try to solve the problem internally.
        *
        * This means that sometimes (depending on hardware, pool size and allocation size) the driver will let us get away with an allocation that exceeds the limits of our descriptor pool.
        * Other times, vkAllocateDescriptorSets will fail and return VK_ERROR_POOL_OUT_OF_MEMORY.
        *
        * This can be particularly frustrating if the allocation succeeds on some machines, but fails on others.
    */
    constexpr VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    if (vkCreateDescriptorPool(mGPUContext->vulkanDevice->getDevice(), &poolInfo, allocationCallbacks, &descriptorPool) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not create descriptor pool");
    }
}

void GPUDescriptorPool::terminate()
{
    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyDescriptorPool(mGPUContext->vulkanDevice->getDevice(), descriptorPool, allocationCallbacks);
}