#include "Graphics/GPU/Shader/GPUShaderDescriptorSets.hpp"

void GPUShaderDescriptorSets::init(const GPUShaderDescriptorSetsData& gpuShaderDescriptorSetsData, Ptr<GPUContext> gpuContext)
{
    mGPUContext = gpuContext;
    mGPUDescriptorData = gpuShaderDescriptorSetsData;
    // LAYOUT

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    u32 bindingIndex = 0;
    FOR_ARRAY(i, mGPUDescriptorData.mUniformBuffers)
    {
        const GPUUniformBuffer& uniformBuffer = mGPUDescriptorData.mUniformBuffers[i];

        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = bindingIndex;
        bindingIndex++;
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
        }

        bindings.push_back(layoutBinding);
    }

    FOR_ARRAY(i, mGPUDescriptorData.mSamplerBindings)
    {
        // const GPUShaderSamplerBinding& samplerBinding = mGPUDescriptorData.mSamplerBindings[i];

        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = bindingIndex;
        bindingIndex++;
        layoutBinding.descriptorCount = 1;
        layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        layoutBinding.pImmutableSamplers = nullptr;

        bindings.push_back(layoutBinding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    constexpr VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    if (vkCreateDescriptorSetLayout(mGPUContext->vulkanDevice->getDevice(), &layoutInfo, allocationCallbacks, &descriptorSetLayout) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not create descrptor set layout");
    }

    // POOL
    std::array<VkDescriptorPoolSize, 3> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[0].descriptorCount = 8;//GPUContext::MAX_FRAMES_IN_FLIGHT * mGPUDescriptorData.mUniformBuffers.size();
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = 8;//GPUContext::MAX_FRAMES_IN_FLIGHT * mGPUDescriptorData.mUniformBuffers.size();
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[2].descriptorCount = 8;//GPUContext::MAX_FRAMES_IN_FLIGHT * mGPUDescriptorData.mSamplerBindings.size();

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = (uint32_t) poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 64;//GPUContext::MAX_FRAMES_IN_FLIGHT * mUniformBuffers.size();

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
    std::vector<VkDescriptorSetLayout> layouts(/*GPUContext::MAX_FRAMES_IN_FLIGHT*/1, descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;//GPUContext::MAX_FRAMES_IN_FLIGHT;
    allocInfo.pSetLayouts = layouts.data();

    // descriptorSets.resize(GPUContext::MAX_FRAMES_IN_FLIGHT);
    descriptorSets.resize(1);
    if (vkAllocateDescriptorSets(mGPUContext->vulkanDevice->getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not allocate [{}] descriptor sets" /*allocInfo.descriptorSetCount*/);
    }

    // for (size_t i = 0; i < GPUContext::MAX_FRAMES_IN_FLIGHT; i++)
    // {
        FOR_ARRAY(j, mGPUDescriptorData.mUniformBuffers)
        {
            const GPUUniformBuffer& uniformBuffer = mGPUDescriptorData.mUniformBuffers[j];

            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffer.getBuffer().getVkBuffer(); // TODO: make double buffered!!
            bufferInfo.offset = 0;
            bufferInfo.range = uniformBuffer.getSize();

            std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[0];
            // descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = j;
            descriptorWrites[0].dstArrayElement = 0;
            switch (uniformBuffer.getGPUUniformBufferData().mType)
            {
            case GPUBufferType::UNIFORM:
                descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                break;
            case GPUBufferType::STORAGE:
                descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                break;
            }
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            mUniformBufferToSet.insert_or_assign(uniformBuffer.getGPUUniformBufferData().mBufferName, 0/*i*/);

            auto descriptorWriteCount = (uint32_t) descriptorWrites.size();
            constexpr uint32_t descriptorCopyCount = 0;
            constexpr VkCopyDescriptorSet* descriptorCopies = nullptr;
            vkUpdateDescriptorSets(mGPUContext->vulkanDevice->getDevice(), descriptorWriteCount, descriptorWrites.data(), descriptorCopyCount, descriptorCopies);
        }

        // FOR_ARRAY(j, mGPUDescriptorData.mSamplerBindings)
        // {
        //     const GPUShaderSamplerBinding& samplerBinding = mGPUDescriptorData.mSamplerBindings[0];
        //     // const GPUShaderSamplerBinding& samplerBinding = mGPUDescriptorData.mSamplerBindings[i];

        //     VkDescriptorImageInfo imageInfo{};
        //     imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        //     imageInfo.imageView = samplerBinding.mGPUTexture->textureImageView;
        //     imageInfo.sampler = samplerBinding.mGPUTexture->textureSampler;

        //     std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

        //     descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        //     descriptorWrites[0].dstSet = descriptorSets[0];
        //     // descriptorWrites[0].dstSet = descriptorSets[i];
        //     descriptorWrites[0].dstBinding = j;
        //     descriptorWrites[0].dstArrayElement = 0;
        //     descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        //     descriptorWrites[0].descriptorCount = 1;
        //     descriptorWrites[0].pImageInfo = &imageInfo;

        //     // mSamplersToSet.insert_or_assign(uniformBuffer.getGPUUniformBufferData().mBufferName, 0/*i*/);

        //     auto descriptorWriteCount = (uint32_t) descriptorWrites.size();
        //     constexpr uint32_t descriptorCopyCount = 0;
        //     constexpr VkCopyDescriptorSet* descriptorCopies = nullptr;
        //     vkUpdateDescriptorSets(mGPUContext->vulkanDevice->getDevice(), descriptorWriteCount, descriptorWrites.data(), descriptorCopyCount, descriptorCopies);
        // }
    // }
}

void GPUShaderDescriptorSets::terminate()
{
    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyDescriptorPool(mGPUContext->vulkanDevice->getDevice(), descriptorPool, allocationCallbacks);
    vkDestroyDescriptorSetLayout(mGPUContext->vulkanDevice->getDevice(), descriptorSetLayout, allocationCallbacks);
}

