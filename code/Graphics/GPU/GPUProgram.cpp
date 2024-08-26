#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/GPU/GPUUniformBuffer.hpp"
#include "Graphics/GPU/GPUInstance.hpp"

GPUProgram::GPUProgram()
{
    vertexShader = new GPUProgramModule(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice);
    fragmentShader = new GPUProgramModule(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice);
};

void GPUProgram::enable() const
{
//	GET_SYSTEM(GPUInterface).enableProgram(mProgramId);
}

void GPUProgram::disable() const
{
//	GET_SYSTEM(GPUInterface).disableProgram(mProgramId);
}

void GPUProgram::initFromFileContents(GPURenderPass* vulkanRenderPass, std::vector<GPUUniformBuffer> uniformBuffers, Ptr<GPUContext> gpuContext, const std::string& vertex, const std::string& fragment)
{
//    mProgramId = GET_SYSTEM(GPUInterface).compileProgram(vertex, fragment);
    mGPUContext = gpuContext;
    mUniformBuffers = uniformBuffers;

    if(!vulkanGraphicsPipeline)
    {
        vulkanGraphicsPipeline = new GPUGraphicsPipeline(vulkanRenderPass, GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain, GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice, GET_SYSTEM(GPUInstance).mGPUContext->vulkanPhysicalDevice);
    }

    createDescriptors();

    // if (!vulkanGraphicsPipeline->initialize(*(mRenderPassData.mShader->vertexShader), *(mRenderPassData.mShader->fragmentShader), mRenderPassData.mShader->descriptorSetLayout))
    // {
    //     CHECK_MSG(false, "Could not initialize Vulkan graphics pipeline");
    // }
}

void GPUProgram::terminate()
{
    vulkanGraphicsPipeline->terminate();

    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyDescriptorPool(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(), descriptorPool, allocationCallbacks);
    vkDestroyDescriptorSetLayout(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(), descriptorSetLayout, allocationCallbacks);
    
    delete vulkanGraphicsPipeline;
}

void GPUProgram::createDescriptors()
{
    // LAYOUT

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    u32 bindingIndex = 0;
    FOR_ARRAY(i, mUniformBuffers)
    {
        const GPUUniformBuffer& uniformBuffer = mUniformBuffers[i];
        
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

    // VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    // samplerLayoutBinding.binding = 1;
    // samplerLayoutBinding.descriptorCount = 1;
    // samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    // samplerLayoutBinding.pImmutableSamplers = nullptr;
    // samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    constexpr VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    if (vkCreateDescriptorSetLayout(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(), &layoutInfo, allocationCallbacks, &descriptorSetLayout) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not create uniform buffer descrptor set layout");
    }

    // POOL
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[0].descriptorCount = GPUContext::MAX_FRAMES_IN_FLIGHT * mUniformBuffers.size();
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = GPUContext::MAX_FRAMES_IN_FLIGHT * mUniformBuffers.size();
    // poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    // poolSizes[1].descriptorCount = GPUContext::MAX_FRAMES_IN_FLIGHT;

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
    if (vkCreateDescriptorPool(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(), &poolInfo, allocationCallbacks, &descriptorPool) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not initialize descriptor pool");
    }

    // SETS
    std::vector<VkDescriptorSetLayout> layouts(GPUContext::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = GPUContext::MAX_FRAMES_IN_FLIGHT;
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(GPUContext::MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not allocate [{}] descriptor sets" /*allocInfo.descriptorSetCount*/);
    }

    for (size_t i = 0; i < GPUContext::MAX_FRAMES_IN_FLIGHT; i++)
    {
        FOR_ARRAY(j, mUniformBuffers)
        {
            const GPUUniformBuffer& uniformBuffer = mUniformBuffers[j];

            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffer.getBuffer().getVkBuffer(); // TODO: make double buffered!!
            bufferInfo.offset = 0;
            bufferInfo.range = uniformBuffer.getSize();

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            // imageInfo.imageView = textureImageView;
            // imageInfo.sampler = textureSampler;

            std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
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

            // descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            // descriptorWrites[1].dstSet = descriptorSets[i];
            // descriptorWrites[1].dstBinding = 1;
            // descriptorWrites[1].dstArrayElement = 0;
            // descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            // descriptorWrites[1].descriptorCount = 1;
            // descriptorWrites[1].pImageInfo = &imageInfo;

            auto descriptorWriteCount = (uint32_t) descriptorWrites.size();
            constexpr uint32_t descriptorCopyCount = 0;
            constexpr VkCopyDescriptorSet* descriptorCopies = nullptr;
            vkUpdateDescriptorSets(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(), descriptorWriteCount, descriptorWrites.data(), descriptorCopyCount, descriptorCopies);
        }
    }
}