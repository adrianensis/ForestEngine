#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"

GPUProgram::GPUProgram()
{
};

void GPUProgram::enable() const
{
//	GET_SYSTEM(GPUInterface).enableProgram(mProgramId);
}

void GPUProgram::disable() const
{
//	GET_SYSTEM(GPUInterface).disableProgram(mProgramId);
}

void GPUProgram::bindSharedBuffer(const GPUSharedBuffer& sharedBuffer)
{
//    GET_SYSTEM(GPUInterface).bindSharedBufferToShader(mProgramId,
    sharedBuffer.getGPUSharedBufferData().mType,
    sharedBuffer.getGPUSharedBufferData().mBufferName,
    sharedBuffer.getBindingPoint());

    mSharedBuffers.push_back(sharedBuffer);
}

void GPUProgram::initFromFileContents(const std::string& vertex, const std::string& fragment)
{
//    mProgramId = GET_SYSTEM(GPUInterface).compileProgram(vertex, fragment);
}

void GPUProgram::initFromFilePaths(const std::string& vertex, const std::string& fragment)
{
	LOG_TRACE()

    std::ifstream vertex_ifs;
	vertex_ifs.open(vertex.c_str(), std::ifstream::in);
	std::string vertexShaderSource((std::istreambuf_iterator<char>(vertex_ifs)), std::istreambuf_iterator<char>());

    std::ifstream fragment_ifs;
	fragment_ifs.open(fragment.c_str(), std::ifstream::in);
	std::string fragmentShaderSource((std::istreambuf_iterator<char>(fragment_ifs)), std::istreambuf_iterator<char>());

    initFromFileContents(vertexShaderSource, fragmentShaderSource);
}

void GPUProgram::terminate()
{
    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyDescriptorPool(GET_SYSTEM(GPUGlobalState).vulkanDevice->getDevice(), descriptorPool, allocationCallbacks);
    vkDestroyDescriptorSetLayout(GET_SYSTEM(GPUGlobalState).vulkanDevice->getDevice(), descriptorSetLayout, allocationCallbacks);
}

void GPUProgram::createDescriptors()
{
    // LAYOUT

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    u32 bindingIndex = 0;
    FOR_ARRAY(i, mSharedBuffers)
    {
        const GPUSharedBuffer& sharedBuffer = mSharedBuffers[i];
        
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = bindingIndex;
        bindingIndex++;
        layoutBinding.descriptorCount = 1;
        layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        switch (sharedBuffer.getGPUSharedBufferData().mType)
        {
        case GPUBufferType::UNIFORM:
            layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            break;
        case GPUBufferType::STORAGE:
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
    if (vkCreateDescriptorSetLayout(GET_SYSTEM(GPUGlobalState).vulkanDevice->getDevice(), &layoutInfo, allocationCallbacks, &descriptorSetLayout) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not create uniform buffer descrptor set layout");
    }

    // POOL
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = GET_SYSTEM(GPUGlobalState).MAX_FRAMES_IN_FLIGHT;
    // poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    // poolSizes[1].descriptorCount = GET_SYSTEM(GPUGlobalState).MAX_FRAMES_IN_FLIGHT;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = (uint32_t) poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = GET_SYSTEM(GPUGlobalState).MAX_FRAMES_IN_FLIGHT;

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
    if (vkCreateDescriptorPool(GET_SYSTEM(GPUGlobalState).vulkanDevice->getDevice(), &poolInfo, allocationCallbacks, &descriptorPool) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not initialize descriptor pool");
    }

    // SETS
    std::vector<VkDescriptorSetLayout> layouts(GET_SYSTEM(GPUGlobalState).MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = GET_SYSTEM(GPUGlobalState).MAX_FRAMES_IN_FLIGHT;
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(GET_SYSTEM(GPUGlobalState).MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(GET_SYSTEM(GPUGlobalState).vulkanDevice->getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not allocate [{}] descriptor sets" /*allocInfo.descriptorSetCount*/);
    }

    for (size_t i = 0; i < GET_SYSTEM(GPUGlobalState).MAX_FRAMES_IN_FLIGHT; i++)
    {
        // const GPUAPI::GPUUniformBuffer& uniformBuffer = uniformBuffers[i];

        VkDescriptorBufferInfo bufferInfo{};
        // bufferInfo.buffer = uniformBuffer.getBuffer().getVkBuffer();
        bufferInfo.offset = 0;
        // bufferInfo.range = sizeof(CameraUniform);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        // imageInfo.imageView = textureImageView;
        // imageInfo.sampler = textureSampler;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
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
        vkUpdateDescriptorSets(GET_SYSTEM(GPUGlobalState).vulkanDevice->getDevice(), descriptorWriteCount, descriptorWrites.data(), descriptorCopyCount, descriptorCopies);
    }
}