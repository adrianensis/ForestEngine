#pragma once

#include "Graphics/GPU/Shader/GPUShaderModule.h"
#include "Graphics/GPU/RenderPass/GPURenderPass.h"
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
    bool initialize(const GPUShaderModule& vertexShader, const GPUShaderModule& fragmentShader, VkDescriptorSetLayout descriptorSetLayout, const GPUVertexInputData& gpuVertexInputData);
    void terminate();
    void bind(const GPUCommandBuffer& vulkanCommandBuffer) const;

private:
    Ptr<GPUContext> mGPUContext;
    GPURenderPass* vulkanRenderPass;
    VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
    VkPipeline mPipeline = VK_NULL_HANDLE;
    GPUVertexInputData mGPUVertexInputData;
public:
    CRGET(PipelineLayout)
    CRGET(Pipeline)
};