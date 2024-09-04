#pragma once

#include "Graphics/GPU/Shader/GPUShaderModule.h"
#include "Graphics/GPU/Core/GPURenderPass.h"
#include "Graphics/GPU/Core/GPUContext.hpp"

class GPUVertexInputData
{
public:
    std::vector<VkVertexInputBindingDescription> mVertexInputBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> mVertexInputAttributeDescriptions;
};

class GPUShaderPipeline
{
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

public:
    GPUShaderPipeline(GPURenderPass* vulkanRenderPass, Ptr<GPUContext> gpuContext);
    const VkPipelineLayout getPipelineLayout() const;
    const VkPipeline getPipeline() const;
    bool initialize(const GPUShaderModule& vertexShader, const GPUShaderModule& fragmentShader, VkDescriptorSetLayout descriptorSetLayout, const GPUVertexInputData& gpuVertexInputData);
    void terminate();
    void bind(const GPUCommandBuffer& vulkanCommandBuffer) const;

private:
    Ptr<GPUContext> mGPUContext;
    GPURenderPass* vulkanRenderPass;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
    GPUVertexInputData mGPUVertexInputData;
};