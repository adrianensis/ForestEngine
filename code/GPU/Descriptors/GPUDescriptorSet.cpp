#include "GPU/Descriptors/GPUDescriptorSet.hpp"

void GPUDescriptorSet::init(const GPUDescriptorSetData& gpuDescriptorSetData, GPUContext* gpuContext)
{
    mGPUContext = gpuContext;
    mGPUDescriptorData = gpuDescriptorSetData;
    // LAYOUT

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    mSamplersBindingIndexOffset = mGPUDescriptorData.mUniformBuffers.size();
    FOR_ARRAY(i, mGPUDescriptorData.mUniformBuffers)
    {
        const GPUUniformBuffer& uniformBuffer = mGPUDescriptorData.mUniformBuffers[i];

        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = i;
        layoutBinding.descriptorCount = 1;
        layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        switch (uniformBuffer.getGPUUniformBufferData().mType)
        {
        case GPUBufferType::UNIFORM:
            layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            break;
        case GPUBufferType::STORAGE:
            layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            break;
        default:
            CHECK_MSG(false, "Not supported buffer type!")
        }

        bindings.push_back(layoutBinding);
    }

    FOR_ARRAY(i, mGPUDescriptorData.mTextureBindings)
    {
        // const GPUShaderTextureBinding& textureBinding = mGPUDescriptorData.mTextureBindings[i];

        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = i + mSamplersBindingIndexOffset;
        layoutBinding.descriptorCount = 1;
        layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        layoutBinding.pImmutableSamplers = nullptr;

        bindings.push_back(layoutBinding);
    }

    VkDescriptorBindingFlags bindingFlags = 
    VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | 
    VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

    VkDescriptorSetLayoutBindingFlagsCreateInfo vkDescriptorSetLayoutBindingFlagsCreateInfo{};
    vkDescriptorSetLayoutBindingFlagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    vkDescriptorSetLayoutBindingFlagsCreateInfo.pBindingFlags = &bindingFlags;
    vkDescriptorSetLayoutBindingFlagsCreateInfo.pNext = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<Core::u32>(bindings.size());
    layoutInfo.pBindings = bindings.data();
    layoutInfo.pNext = &vkDescriptorSetLayoutBindingFlagsCreateInfo;
    layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

    constexpr VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    if (vkCreateDescriptorSetLayout(mGPUContext->vulkanDevice->getDevice(), &layoutInfo, allocationCallbacks, &descriptorSetLayout) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not create descrptor set layout");
    }

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
    if (vkCreateDescriptorPool(mGPUContext->vulkanDevice->getDevice(), &poolInfo, allocationCallbacks, &descriptorPool) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not create descriptor pool");
    }

    // SETS
    std::vector<VkDescriptorSetLayout> layouts(GPUContext::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

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

    size_t numBuffers = mGPUDescriptorData.mUniformBuffers.size() * GPUContext::MAX_FRAMES_IN_FLIGHT;
    size_t numImages = mGPUDescriptorData.mTextureBindings.size() * GPUContext::MAX_FRAMES_IN_FLIGHT;
    
    writes.reserve(numBuffers + numImages);
    bufferInfos.reserve(numBuffers);
    imageInfos.reserve(numImages);

    for (size_t i = 0; i < GPUContext::MAX_FRAMES_IN_FLIGHT; i++)
    {
        FOR_ARRAY(j, mGPUDescriptorData.mUniformBuffers)
        {
            const GPUUniformBuffer& uniformBuffer = mGPUDescriptorData.mUniformBuffers[j];

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

        FOR_ARRAY(j, mGPUDescriptorData.mTextureBindings)
        {
            const GPUShaderTextureBinding& textureBinding = mGPUDescriptorData.mTextureBindings[j];

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
    vkDestroyDescriptorSetLayout(mGPUContext->vulkanDevice->getDevice(), descriptorSetLayout, allocationCallbacks);
}