#include "Graphics/Material/Shader/Shader.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"

#include "Graphics/GPU/GPUTexture.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/Mesh/GPUMesh.hpp"
#include "Graphics/GPU/GPUInstance.hpp"
#include "Graphics/Model/Model.hpp"
#include "Core/Config/Paths.hpp"
#include "Core/File/FileUtils.hpp"
#include <cstdlib>

void Shader::init()
{

    GPUStructDefinition propertiesBlockStructDefinition =
    {
        ShaderPropertiesBlockNames::smPropertiesBlockStructName,
        {
            generateMaterialPropertiesBlock()
        }
    };

    GPUDataType propertiesBlockStructDataType =
    {
        propertiesBlockStructDefinition.mName,
        propertiesBlockStructDefinition.getTypeSizeInBytes(),
        GPUPrimitiveDataType::STRUCT
    };

    GPUUniformBufferData propertiesBlockUniformBufferData =
    {
        GPUBufferType::STORAGE,
        {
            {{GPUStorage::UNIFORM, propertiesBlockStructDataType, ShaderPropertiesBlockNames::smPropertiesBlockArrayName}, "", " "}
        },
        ShaderPropertiesBlockNames::smPropertiesBlockBufferName,
        ShaderPropertiesBlockNames::smPropertiesBlockInstanceName
    };

    mShaderData.mPropertiesBlockStructDefinition = propertiesBlockStructDefinition;
    mShaderData.mPropertiesBlockUniformBufferData = propertiesBlockUniformBufferData;

    registerTextures();
}

void Shader::init(const ShaderData& shaderData)
{
    mShaderData = shaderData;
}

void Shader::terminate()
{

}

std::vector<GPUStructDefinition::GPUStructVariable> Shader::generateMaterialPropertiesBlock()
{
    std::vector<GPUStructDefinition::GPUStructVariable> propertiesBlock =
    {
        {GPUBuiltIn::PrimitiveTypes::mInt, "_emptyStructFixHack"}
    };

    return propertiesBlock;
}

void Shader::enable() const
{
	PROFILER_CPU()
    u32 textureUnit = 0;
    FOR_MAP(it, mShaderData.mFramebufferBindings)
    {
//        GET_SYSTEM(GPUInterface).enableTexture(it->second.mTextureID, textureUnit, it->second.mStage);
        textureUnit++;
    }
}

void Shader::disable() const
{
	PROFILER_CPU()
    u32 textureUnit = 0;
    FOR_MAP(it, mShaderData.mFramebufferBindings)
    {
//        GET_SYSTEM(GPUInterface).disableTexture(textureUnit, it->second.mStage);
        textureUnit++;
    }
}

bool Shader::hasFramebufferBinding(HashedString bindingName) const
{
    return mShaderData.mFramebufferBindings.contains(bindingName);
}

void Shader::bindTextures(Ptr<GPUProgram> gpuProgram, const std::unordered_map<HashedString, PoolHandler<GPUTexture>>& textures) const
{
    // gpuProgram->enable();

    // u32 textureUnit = 0;
    // FOR_MAP(it, mShaderData.mFramebufferBindings)
    // {
    //     gpuProgram->bindUniformValue<i32>(GPUBuiltIn::Uniforms::getSampler(it->second.mSamplerName).mName, textureUnit);
    //     textureUnit++;
    // }

    // // Init all samplers to disable
    // FOR_MAP(it, mShaderData.mTextures)
    // {
    //     // NOTE: We reserve position 0 to represent NULL
    //     gpuProgram->bindUniformValue<u32>(GPUBuiltIn::Uniforms::getTextureHandler(*it).mName, 0);
    // }

    // FOR_MAP(it, textures)
    // {
    //     // NOTE: We reserve position 0 to represent NULL
    //     gpuProgram->bindUniformValue<u32>(GPUBuiltIn::Uniforms::getTextureHandler(it->first).mName, it->second->getID() + 1);
    // }

    // gpuProgram->disable();
}

void Shader::addFramebufferBinding(const FramebufferBinding& framebufferBinding)
{
    mShaderData.mFramebufferBindings.insert_or_assign(framebufferBinding.mSamplerName, framebufferBinding);
}

void Shader::generateShaderGenerationData(ShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
{
}

OwnerPtr<GPUProgram> Shader::compileShader(const ShaderCompileData& shaderCompileData)
{
    mShaderData.mShaderCompileData = shaderCompileData;

    GPUProgramDescriptorsData gpuProgramDescriptorsData = createDescriptors(mShaderData.mShaderCompileData);

    OwnerPtr<GPUProgram> gpuProgram = OwnerPtr<GPUProgram>::newObject();

    GPUVertexBuffersContainer gpuVertexBuffersContainer;
    mShaderData.mShaderCompileData.mMesh->populateGPUVertexBuffersContainer(gpuVertexBuffersContainer, false); 

    ShaderBuilder sbVert;
    ShaderBuilder sbFrag;
    createVertexShader(sbVert, gpuVertexBuffersContainer, gpuProgramDescriptorsData);
    createFragmentShader(sbFrag, gpuVertexBuffersContainer, gpuProgramDescriptorsData);

    std::string stringShaderVert = sbVert.getCode();
    std::string shaderPathVert = Paths::mOutputShaders.get() + mShaderData.mShaderCompileData.id.get() + "_" + mShaderData.mShaderCompileData.label.get() + ".vert";
    FileUtils::writeFile(shaderPathVert, [stringShaderVert](std::ofstream& file)
    {
        file << stringShaderVert;
    });

    system(std::string("glslc "s + shaderPathVert + " -o "s + shaderPathVert + ".spv").c_str());
    
    std::vector<byte> stringShaderVertSpvBinary;
    FileUtils::readFileBinaryData(shaderPathVert + ".spv", stringShaderVertSpvBinary);

    std::string stringShaderFrag = sbFrag.getCode();
    std::string shaderPathFrag = Paths::mOutputShaders.get() + mShaderData.mShaderCompileData.id.get() + "_" + mShaderData.mShaderCompileData.label.get() + ".frag";
    FileUtils::writeFile(shaderPathFrag, [stringShaderFrag](std::ofstream& file)
    {
        file << stringShaderFrag;
    });

    system(std::string("glslc "s + shaderPathFrag + " -o "s + shaderPathFrag + ".spv").c_str());
    
    std::vector<byte> stringShaderFragSpvBinary;
    FileUtils::readFileBinaryData(shaderPathFrag + ".spv", stringShaderFragSpvBinary);

    gpuProgram->initFromFileContents(mShaderData.mShaderCompileData.vulkanRenderPass, gpuProgramDescriptorsData, mShaderData.mShaderCompileData.mUniformBuffers, gpuVertexBuffersContainer.getVertexBuffers(), GET_SYSTEM(GPUInstance).mGPUContext, stringShaderVertSpvBinary, stringShaderFragSpvBinary);

    return gpuProgram;
}

GPUProgramDescriptorsData Shader::createDescriptors(ShaderCompileData& shaderCompileData)
{
    GPUProgramDescriptorsData gpuProgramDescriptorsData;
    // LAYOUT

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    u32 bindingIndex = 0;
    FOR_ARRAY(i, shaderCompileData.mUniformBuffers)
    {
        const GPUUniformBuffer& uniformBuffer = shaderCompileData.mUniformBuffers[i];
        
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
    if (vkCreateDescriptorSetLayout(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(), &layoutInfo, allocationCallbacks, &gpuProgramDescriptorsData.descriptorSetLayout) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not create uniform buffer descrptor set layout");
    }

    // POOL
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[0].descriptorCount = GPUContext::MAX_FRAMES_IN_FLIGHT * shaderCompileData.mUniformBuffers.size();
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = GPUContext::MAX_FRAMES_IN_FLIGHT * shaderCompileData.mUniformBuffers.size();
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
    if (vkCreateDescriptorPool(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(), &poolInfo, allocationCallbacks, &gpuProgramDescriptorsData.descriptorPool) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not initialize descriptor pool");
    }

    // SETS
    std::vector<VkDescriptorSetLayout> layouts(GPUContext::MAX_FRAMES_IN_FLIGHT, gpuProgramDescriptorsData.descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = gpuProgramDescriptorsData.descriptorPool;
    allocInfo.descriptorSetCount = GPUContext::MAX_FRAMES_IN_FLIGHT;
    allocInfo.pSetLayouts = layouts.data();

    gpuProgramDescriptorsData.descriptorSets.resize(GPUContext::MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(), &allocInfo, gpuProgramDescriptorsData.descriptorSets.data()) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Could not allocate [{}] descriptor sets" /*allocInfo.descriptorSetCount*/);
    }

    for (size_t i = 0; i < GPUContext::MAX_FRAMES_IN_FLIGHT; i++)
    {
        FOR_ARRAY(j, shaderCompileData.mUniformBuffers)
        {
            const GPUUniformBuffer& uniformBuffer = shaderCompileData.mUniformBuffers[j];

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
            descriptorWrites[0].dstSet = gpuProgramDescriptorsData.descriptorSets[i];
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

            gpuProgramDescriptorsData.mUniformBufferToSet.insert_or_assign(uniformBuffer.getGPUUniformBufferData().mBufferName, 0/*i*/);

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

    return gpuProgramDescriptorsData;
}