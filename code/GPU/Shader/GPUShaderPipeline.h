#pragma once

#include "GPU/Shader/GPUShaderModule.h"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Shader/GPUShaderDescriptorSets.hpp"
#include "GPU/Buffer/GPUVertexBuffer.hpp"

class GPURenderPass;

class GPUVertexInputData
{
public:
    std::vector<VkVertexInputBindingDescription> mVertexInputBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> mVertexInputAttributeDescriptions;
};

class GPUShaderPipelineDepthStencilData
{
public:
    bool mDepthTestEnable = VK_TRUE;
    bool mDepthWriteEnable = VK_TRUE;
    VkCompareOp mDepthCompareOp = VK_COMPARE_OP_LESS;
    bool mDepthBoundsTestEnable = VK_FALSE;
    bool mStencilTestEnable = VK_FALSE;
    VkStencilOpState mStencilFront;
    VkStencilOpState mStencilBack;
    float mMinDepthBounds = 0;
    float mMaxDepthBounds = 0;
};

class GPUShaderPipelineData
{
public:
    GPUShaderDescriptorSetsData mGPUShaderDescriptorSetsData;
    std::vector<GPUVertexBuffer> mVertexInputBuffers;
    GPUShaderPipelineDepthStencilData mGPUShaderPipelineDepthStencilData;
};

class GPUShaderCompile
{
public:
    GPUShaderDescriptorSetsData mGPUShaderDescriptorSetsData;
    std::vector<GPUVertexBuffer> mVertexInputBuffers;
    GPUShaderPipelineDepthStencilData mGPUShaderPipelineDepthStencilData;
};

class GPUShaderPipeline
{
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

public:
    void init(const GPUShaderPipelineData& gpuGPUShaderPipelineData, GPURenderPass* renderPass, GPUContext* gpuContext);
    void terminate();
    void bind(const GPUCommandBuffer& vulkanCommandBuffer) const;
    void compile(const GPUShaderModuleData& vertex, const GPUShaderModuleData& fragment);
    void enable() const;
    void disable() const;
private:
    Core::OwnerPtr<GPUShaderDescriptorSets> mGPUShaderDescriptorSets;
    GPUShaderModule vertexShader;
    GPUShaderModule fragmentShader;
    GPUVertexInputData mGPUVertexInputData;
    GPURenderPass* mRenderPass = nullptr;
    GPUContext* mGPUContext = nullptr;
    VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
    VkPipeline mPipeline = VK_NULL_HANDLE;
    GPUShaderPipelineData mGPUShaderPipelineData;
public:
    CGET(GPUShaderDescriptorSets)
    CRGET(PipelineLayout)
    CRGET(Pipeline)
};