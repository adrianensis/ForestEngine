#include "Graphics/GPU/Shader/GPUShader.hpp"
#include "Graphics/GPU/Buffer/GPUBuffersContainer.hpp"
#include "Graphics/GPU/Buffer/GPUUniformBuffer.hpp"
#include "Graphics/GPU/GPUInstance.hpp"

GPUShader::GPUShader()
{
    vertexShader = new GPUShaderModule(GET_SYSTEM(GPUInstance).mGPUContext);
    fragmentShader = new GPUShaderModule(GET_SYSTEM(GPUInstance).mGPUContext);
};

void GPUShader::enable() const
{
//	GET_SYSTEM(GPUInterface).enableProgram(mProgramId);
}

void GPUShader::disable() const
{
//	GET_SYSTEM(GPUInterface).disableProgram(mProgramId);
}

void GPUShader::initFromFileContents(GPURenderPass* vulkanRenderPass, GPUShaderDescriptorSets* gpuShaderDescriptorSets, const std::vector<GPUVertexBuffer>& vertexInputBuffers, Ptr<GPUContext> gpuContext, const std::vector<byte>& vertex, const std::vector<byte>& fragment)
{
//    mProgramId = GET_SYSTEM(GPUInterface).compileProgram(vertex, fragment);
    mGPUContext = gpuContext;
    // mUniformBuffers = uniformBuffers;
    mGPUDescriptor = gpuShaderDescriptorSets;

    if (!vertexShader->initialize(vertex)) {
        CHECK_MSG(false, "Could not initialize vertex shader");
        // return false;
    }
    if (!fragmentShader->initialize(fragment)) {
        CHECK_MSG(false, "Could not initialize fragment shader");
        // return false;
    }

    if(!gpuShaderPipeline)
    {
        gpuShaderPipeline = new GPUShaderPipeline(vulkanRenderPass, GET_SYSTEM(GPUInstance).mGPUContext);
    }

    // createDescriptors();

    GPUVertexInputData gpuVertexInputData;
    gpuVertexInputData.mVertexInputBindingDescriptions.resize(vertexInputBuffers.size());
    gpuVertexInputData.mVertexInputAttributeDescriptions.resize(vertexInputBuffers.size());
    FOR_ARRAY(i, vertexInputBuffers)
    {
        const GPUVertexBuffer& gpuVertexBuffer = vertexInputBuffers[i];
        // gpuVertexBuffer.mData.mGPUVariableData.

        gpuVertexInputData.mVertexInputBindingDescriptions[i].binding = i;
        gpuVertexInputData.mVertexInputBindingDescriptions[i].stride = gpuVertexBuffer.mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes;
        gpuVertexInputData.mVertexInputBindingDescriptions[i].inputRate = gpuVertexBuffer.mData.mInstanceDivisor == 0 ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE;
    
        gpuVertexInputData.mVertexInputAttributeDescriptions[i].binding = i;
        gpuVertexInputData.mVertexInputAttributeDescriptions[i].location = i;
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
                format = VK_FORMAT_R32_SINT;
                break;
            case 2:
                format = VK_FORMAT_R32G32_SINT;
                break;
            case 3:
                format = VK_FORMAT_R32G32B32_SINT;
                break;
            case 4:
                format = VK_FORMAT_R32G32B32A32_SINT;
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

        gpuVertexInputData.mVertexInputAttributeDescriptions[i].format = format;//VK_FORMAT_R32G32B32_SFLOAT;
        // gpuVertexInputData.mVertexInputAttributeDescriptions[i].offset = offsetof(Vertex, position);
    }

    if (!gpuShaderPipeline->initialize(*vertexShader, *fragmentShader, mGPUDescriptor->descriptorSetLayout, gpuVertexInputData))
    {
        CHECK_MSG(false, "Could not initialize Vulkan graphics pipeline");
    }
}

void GPUShader::terminate()
{
    gpuShaderPipeline->terminate();

    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyDescriptorPool(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(), mGPUDescriptor->descriptorPool, allocationCallbacks);
    vkDestroyDescriptorSetLayout(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(), mGPUDescriptor->descriptorSetLayout, allocationCallbacks);
    
    delete gpuShaderPipeline;
}

