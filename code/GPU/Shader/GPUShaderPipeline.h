#pragma once

#include "GPU/Shader/GPUShaderModule.h"
#include "GPU/RenderPass/GPURenderPass.h"
#include "GPU/Core/GPUContext.hpp"

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
    GPUShaderPipeline(GPURenderPass* vulkanRenderPass, WeakPtr<GPUContext> gpuContext);
    bool init(const GPUShaderModule& vertexShader, const GPUShaderModule& fragmentShader, VkDescriptorSetLayout descriptorSetLayout, const GPUVertexInputData& gpuVertexInputData);
    void terminate();
    void bind(const GPUCommandBuffer& vulkanCommandBuffer) const;

private:
    GPURenderPass* vulkanRenderPass;
    WeakPtr<GPUContext> mGPUContext;
    VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
    VkPipeline mPipeline = VK_NULL_HANDLE;
    GPUVertexInputData mGPUVertexInputData;
public:
    CRGET(PipelineLayout)
    CRGET(Pipeline)
};