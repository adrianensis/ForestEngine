#include "GPU/Descriptors/GPUDescriptorSet.hpp"

void GPUDescriptorSet::init(const GPUDescriptorLayoutData& gpuDescriptorLayoutData, GPUContext* gpuContext)
{
    mGPUContext = gpuContext;
    mGPUDescriptorLayout.init(gpuDescriptorLayoutData, mGPUContext);
    mSamplersBindingIndexOffset = mGPUDescriptorLayout.mGPUDescriptorLayoutData.mUniformBuffers.size();

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
    poolSizes[0].descriptorCount = 20; // TODO: Set value to indexingProps.maxDescriptorSetUpdateAfterBindStorageBuffers;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = 20; // TODO: Set value to indexingProps.maxDescriptorSetUpdateAfterBindUniformBuffers;
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[2].descriptorCount = 20; // TODO: Set value to indexingProps.maxDescriptorSetUpdateAfterBindSamplers;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    poolInfo.poolSizeCount = poolTypesCount;
    poolInfo.pPoolSizes = poolSizes.data();
    // TODO: select a correct poolInfo.maxSets number
    poolInfo.maxSets = GPUContext::MAX_FRAMES_IN_FLIGHT;

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

    // SETS
    std::vector<VkDescriptorSetLayout> layouts(GPUContext::MAX_FRAMES_IN_FLIGHT, mGPUDescriptorLayout.descriptorSetLayout);

    VkDescriptorSetVariableDescriptorCountAllocateInfoEXT count_info;
    count_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT;
    count_info.pNext = nullptr;
    Core::u32 max_binding = indexingProps.maxDescriptorSetUpdateAfterBindSampledImages - 1;
    count_info.descriptorSetCount = GPUContext::MAX_FRAMES_IN_FLIGHT;
    count_info.pDescriptorCounts = &max_binding;

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = &count_info;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = GPUContext::MAX_FRAMES_IN_FLIGHT;
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(GPUContext::MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(mGPUContext->vulkanDevice->getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not allocate [{}] descriptor sets", allocInfo.descriptorSetCount);
    }

    update();
}

void GPUDescriptorSet::update()
{
    std::vector<VkWriteDescriptorSet> writes;

    std::vector<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkDescriptorImageInfo> imageInfos;

    size_t numBuffers = mGPUDescriptorLayout.mGPUDescriptorLayoutData.mUniformBuffers.size() * GPUContext::MAX_FRAMES_IN_FLIGHT;
    size_t numImages = mGPUDescriptorLayout.mGPUDescriptorLayoutData.mTextureBindings.size() * GPUContext::MAX_FRAMES_IN_FLIGHT;
    
    writes.reserve(numBuffers + numImages);
    bufferInfos.reserve(numBuffers);
    imageInfos.reserve(numImages);

    for (size_t i = 0; i < GPUContext::MAX_FRAMES_IN_FLIGHT; i++)
    {
        FOR_ARRAY(j, mGPUDescriptorLayout.mGPUDescriptorLayoutData.mUniformBuffers)
        {
            const GPUUniformBuffer& uniformBuffer = mGPUDescriptorLayout.mGPUDescriptorLayoutData.mUniformBuffers[j];

            VkDescriptorBufferInfo& bufferInfo = bufferInfos.emplace_back();
            bufferInfo.buffer = uniformBuffer.getBuffer().getVkBuffer(); // TODO: make double buffered!!
            bufferInfo.offset = 0;
            bufferInfo.range = uniformBuffer.getSize();

            VkWriteDescriptorSet& descriptorWrite = writes.emplace_back();
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[i];
            descriptorWrite.dstBinding = j;
            descriptorWrite.dstArrayElement = 0;
            switch (uniformBuffer.getGPUUniformBufferData().mType)
            {
            case GPUBufferType::UNIFORM:
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                break;
            case GPUBufferType::STORAGE:
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                break;
            default:
                CHECK_MSG(false, "Not supported buffer type!")
            }
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;

            mGPUDescriptorSetBindings.mBindings.emplace(uniformBuffer.getGPUUniformBufferData().mBufferName,descriptorWrite.dstBinding);
            mGPUDescriptorSetBindings.mSets.emplace(uniformBuffer.getGPUUniformBufferData().mBufferName,i);
        }

        FOR_ARRAY(j, mGPUDescriptorLayout.mGPUDescriptorLayoutData.mTextureBindings)
        {
            const GPUShaderTextureBinding& textureBinding = mGPUDescriptorLayout.mGPUDescriptorLayoutData.mTextureBindings[j];

            VkDescriptorImageInfo& imageInfo = imageInfos.emplace_back();
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = textureBinding.mGPUTexture->mTextureImageView;
            imageInfo.sampler = textureBinding.mGPUTexture->mTextureSampler;

            VkWriteDescriptorSet& descriptorWrite = writes.emplace_back();
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[i];
            descriptorWrite.dstBinding = j + mSamplersBindingIndexOffset;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &imageInfo;

            mGPUDescriptorSetBindings.mBindings.emplace(textureBinding.mName, descriptorWrite.dstBinding);
            mGPUDescriptorSetBindings.mSets.emplace(textureBinding.mName,i);
        }
    }

    auto descriptorWriteCount = (Core::u32) writes.size();
    constexpr Core::u32 descriptorCopyCount = 0;
    constexpr VkCopyDescriptorSet* descriptorCopies = nullptr;
    vkUpdateDescriptorSets(mGPUContext->vulkanDevice->getDevice(), descriptorWriteCount, writes.data(), descriptorCopyCount, descriptorCopies);
}

void GPUDescriptorSet::terminate()
{
    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyDescriptorPool(mGPUContext->vulkanDevice->getDevice(), descriptorPool, allocationCallbacks);
    vkDestroyDescriptorSetLayout(mGPUContext->vulkanDevice->getDevice(), mGPUDescriptorLayout.descriptorSetLayout, allocationCallbacks);
}