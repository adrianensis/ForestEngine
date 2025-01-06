#include "GPU/Shader/GPUShader.hpp"
#include "GPU/Buffer/GPUBuffersContainer.hpp"
#include "GPU/Buffer/GPUUniformBuffer.hpp"
#include "GPU/GPUInstance.hpp"

void GPUShader::enable() const
{
//	GET_SYSTEM(GPUInterface).enableProgram(mProgramId);
    const GPUCommandBuffer* vulkanCommandBuffer = GET_SYSTEM(GPUInstance).mGPUContext->vulkanCommandBuffers[GET_SYSTEM(GPUInstance).mGPUContext->currentFrame];
    gpuShaderPipeline->bind(*vulkanCommandBuffer);

    VkDescriptorSet descriptorSet = mGPUShaderDescriptorSets->descriptorSets[mGPUContext->currentFrame];
    VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    VkPipelineLayout pipelineLayout = gpuShaderPipeline->getPipelineLayout();
    constexpr u32 firstSet = 0;
    constexpr u32 descriptorSetCount = 1;
    constexpr u32 dynamicOffsetCount = 0;
    constexpr u32* dynamicOffsets = nullptr;
    vkCmdBindDescriptorSets(vulkanCommandBuffer->getVkCommandBuffer(), pipelineBindPoint, pipelineLayout, firstSet, descriptorSetCount, &descriptorSet, dynamicOffsetCount, dynamicOffsets);
}

void GPUShader::disable() const
{
//	GET_SYSTEM(GPUInterface).disableProgram(mProgramId);
}

void GPUShader::init(GPURenderPass* vulkanRenderPass, const GPUShaderDescriptorSetsData& gpuShaderDescriptorSetsData, const std::vector<GPUVertexBuffer>& vertexInputBuffers, WeakPtr<GPUContext> gpuContext)
{
//    mProgramId = GET_SYSTEM(GPUInterface).compileProgram(vertex, fragment);
    mGPUContext = gpuContext;
    // mUniformBuffers = uniformBuffers;
    mGPUShaderDescriptorSets = new GPUShaderDescriptorSets();
    mGPUShaderDescriptorSets->init(gpuShaderDescriptorSetsData, mGPUContext);

    if(!gpuShaderPipeline)
    {
        gpuShaderPipeline = new GPUShaderPipeline(vulkanRenderPass, mGPUContext);
    }

    // createDescriptors();

    mGPUVertexInputData.mVertexInputBindingDescriptions.resize(vertexInputBuffers.size());
    mGPUVertexInputData.mVertexInputAttributeDescriptions.resize(vertexInputBuffers.size());
    FOR_ARRAY(i, vertexInputBuffers)
    {
        const GPUVertexBuffer& gpuVertexBuffer = vertexInputBuffers[i];
        // gpuVertexBuffer.mData.mGPUVariableData.

        mGPUVertexInputData.mVertexInputBindingDescriptions[i].binding = i;
        mGPUVertexInputData.mVertexInputBindingDescriptions[i].stride = gpuVertexBuffer.mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes;
        mGPUVertexInputData.mVertexInputBindingDescriptions[i].inputRate = gpuVertexBuffer.mData.mInstanceDivisor == 0 ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE;
    
        mGPUVertexInputData.mVertexInputAttributeDescriptions[i].binding = i;
        mGPUVertexInputData.mVertexInputAttributeDescriptions[i].location = i;
        VkFormat format = VK_FORMAT_UNDEFINED;
        switch (gpuVertexBuffer.mData.mGPUVariableData.mGPUDataType.mPrimitiveDataType)
        {
        case GPUPrimitiveDataType::FLOAT:
            switch (gpuVertexBuffer.mData.mGPUVariableData.mGPUDataType.getSizePrimitiveType())
            {
            case 1:
                format = VK_FORMAT_R32_SFLOAT;
                break;
            case 2:
                format = VK_FORMAT_R32G32_SFLOAT;
                break;
            case 3:
                format = VK_FORMAT_R32G32B32_SFLOAT;
                break;
            case 4:
                format = VK_FORMAT_R32G32B32A32_SFLOAT;
                break;
            default:
                CHECK_MSG(false, "ERROR")
                break;
            }
            break;
        case GPUPrimitiveDataType::INT:
            switch (gpuVertexBuffer.mData.mGPUVariableData.mGPUDataType.getSizePrimitiveType())
            {
            case 1:
                format = VK_FORMAT_R32_UINT;
                break;
            case 2:
                format = VK_FORMAT_R32G32_UINT;
                break;
            case 3:
                format = VK_FORMAT_R32G32B32_UINT;
                break;
            case 4:
                format = VK_FORMAT_R32G32B32A32_UINT;
                break;
            default:
                CHECK_MSG(false, "ERROR")
                break;
            }
            break;
        
        default:
            CHECK_MSG(false, "ERROR")
            break;
        }

        mGPUVertexInputData.mVertexInputAttributeDescriptions[i].format = format;//VK_FORMAT_R32G32B32_SFLOAT;
        // mGPUVertexInputData.mVertexInputAttributeDescriptions[i].offset = offsetof(Vertex, position);
    }
}

void GPUShader::compile(const std::vector<byte>& vertex, const std::vector<byte>& fragment)
{
    if (!vertexShader.init(mGPUContext, vertex))
    {
        CHECK_MSG(false, "Could not initialize vertex shader");
        // return false;
    }
    if (!fragmentShader.init(mGPUContext, fragment))
    {
        CHECK_MSG(false, "Could not initialize fragment shader");
        // return false;
    }

    if (!gpuShaderPipeline->init(vertexShader, fragmentShader, mGPUShaderDescriptorSets->descriptorSetLayout, mGPUVertexInputData))
    {
        CHECK_MSG(false, "Could not initialize Vulkan graphics pipeline");
    }
}

void GPUShader::terminate()
{
    gpuShaderPipeline->terminate();

    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyDescriptorPool(mGPUContext->vulkanDevice->getDevice(), mGPUShaderDescriptorSets->descriptorPool, allocationCallbacks);
    vkDestroyDescriptorSetLayout(mGPUContext->vulkanDevice->getDevice(), mGPUShaderDescriptorSets->descriptorSetLayout, allocationCallbacks);
    
    delete gpuShaderPipeline;
}

