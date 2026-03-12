#include "GPU/Descriptors/GPUDescriptorSet.hpp"
#include "Core/CoreBase.hpp"
#include "GPU/Texture/GPUTexture.hpp"

void GPUDescriptorSet::init(const GPUDescriptorLayoutData& gpuDescriptorLayoutData, const GPUDescriptorPool& gpuDescriptorPool, GPUTextureManager* gpuTextureManager, GPUContext* gpuContext)
{
    mGPUContext = gpuContext;
    mGPUTextureManager = gpuTextureManager;
    mGPUDescriptorLayout.init(gpuDescriptorLayoutData, mGPUContext);

    VkPhysicalDeviceDescriptorIndexingProperties indexingProps{};
    indexingProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;

    VkPhysicalDeviceProperties2 deviceProps{};
    deviceProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    deviceProps.pNext = &indexingProps;

    vkGetPhysicalDeviceProperties2(mGPUContext->vulkanPhysicalDevice->getPhysicalDevice(), &deviceProps);

    // SETS
    std::vector<VkDescriptorSetLayout> layouts(GPUContext::MAX_FRAMES_IN_FLIGHT, mGPUDescriptorLayout.descriptorSetLayout);

    VkDescriptorSetVariableDescriptorCountAllocateInfoEXT count_info;
    count_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT;
    count_info.pNext = nullptr;
    std::array<uint32_t, GPUContext::MAX_FRAMES_IN_FLIGHT> descriptorCounts = {1024, 1024};//indexingProps.maxDescriptorSetUpdateAfterBindSampledImages - 1;
    count_info.descriptorSetCount = GPUContext::MAX_FRAMES_IN_FLIGHT;
    count_info.pDescriptorCounts = descriptorCounts.data();

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = gpuDescriptorPool.descriptorPool;
    allocInfo.descriptorSetCount = GPUContext::MAX_FRAMES_IN_FLIGHT;
    allocInfo.pSetLayouts = layouts.data();

    if (mGPUDescriptorLayout.mGPUDescriptorLayoutData.mIsBindless) 
    {
        allocInfo.pNext = &count_info;
    }

    descriptorSets.resize(GPUContext::MAX_FRAMES_IN_FLIGHT);
    VkResult result = vkAllocateDescriptorSets(mGPUContext->vulkanDevice->getDevice(), &allocInfo, descriptorSets.data());

    if (result != VK_SUCCESS)
    {
        if (result == VK_ERROR_OUT_OF_POOL_MEMORY) {
            // You requested 1024 * 2 samplers, but the pool doesn't have them.
            CHECK_MSG(false, "VK_ERROR_OUT_OF_POOL_MEMORY");
        } else if (result == VK_ERROR_FRAGMENTED_POOL) {
            // Pool has space, but not a contiguous block (unlikely for new pools).
            CHECK_MSG(false, "VK_ERROR_FRAGMENTED_POOL");
        }
        CHECK_MSG(false, "Vulkan Error: {}", (int)result);
    }

    update();
}

void GPUDescriptorSet::update()
{
    Core::u32 samplersBindingIndexOffset = mGPUDescriptorLayout.mGPUDescriptorLayoutData.mUniformBuffers.size();

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
        }

        FOR_ARRAY(j, mGPUDescriptorLayout.mGPUDescriptorLayoutData.mTextureBindings)
        {
            const GPUTextureBinding& textureBinding = mGPUDescriptorLayout.mGPUDescriptorLayoutData.mTextureBindings[j];

            const GPUTexture& texture = mGPUTextureManager->getTexture(textureBinding.mGPUTextureHandle);

            VkDescriptorImageInfo& imageInfo = imageInfos.emplace_back();
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = texture.mTextureImageView;
            imageInfo.sampler = texture.mTextureSampler;

            VkWriteDescriptorSet& descriptorWrite = writes.emplace_back();
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[i];
            descriptorWrite.dstBinding = j + samplersBindingIndexOffset;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &imageInfo;
        }
    }

    auto descriptorWriteCount = (Core::u32) writes.size();
    constexpr Core::u32 descriptorCopyCount = 0;
    constexpr VkCopyDescriptorSet* descriptorCopies = nullptr;
    vkUpdateDescriptorSets(mGPUContext->vulkanDevice->getDevice(), descriptorWriteCount, writes.data(), descriptorCopyCount, descriptorCopies);
}

void GPUDescriptorSet::updateBindlessSlot(Core::Slot slot, const GPUTexture& texture)
{
    if (mGPUDescriptorLayout.mGPUDescriptorLayoutData.mIsBindless) 
    {
        // TODO: No need double buffering for bindless textures descriptor,
        // Because of the UPDATE_AFTER_BIND flag, you can safely write a new texture into an empty slot
        // in the array even if the GPU is currently drawing other things from that same set.
        for (size_t i = 0; i < GPUContext::MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = texture.mTextureImageView;
            imageInfo.sampler = texture.mTextureSampler;

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            // This is your Global Bindless Set
            descriptorWrite.dstSet = descriptorSets[mGPUContext->currentFrame]; 
            // This is the binding index (e.g., the one you set to 1024 count)
            descriptorWrite.dstBinding = mGPUDescriptorLayout.mGPUDescriptorLayoutData.mTextureBindings.size(); 
            descriptorWrite.dstArrayElement = slot.getSlot();
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(mGPUContext->vulkanDevice->getDevice(), 1, &descriptorWrite, 0, nullptr);
        }
    }
}

void GPUDescriptorSet::terminate()
{
    mGPUDescriptorLayout.terminate();
}