#pragma once

#include "Core/Minimal.hpp"
#include "GPU/Buffer/GPUUniformBuffer.hpp"
#include "GPU/Buffer/GPUVertexBuffer.hpp"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Shader/GPUShaderDescriptorSets.hpp"
#include "GPU/Shader/GPUShaderPipeline.h"

class GPURenderPass;

class GPUShader
{    
public:

    GPUShader();

    void init(GPURenderPass* vulkanRenderPass, const GPUShaderDescriptorSetsData& gpuShaderDescriptorSetsData, const std::vector<GPUVertexBuffer>& vertexInputBuffers, WeakPtr<GPUContext> gpuContext);
    void compile(const std::vector<byte>& vertex, const std::vector<byte>& fragment);
    void terminate();
    void enable() const;
    void disable() const;
private:
	u32 mProgramId = 0;
    WeakPtr<GPUContext> mGPUContext;
    GPUShaderPipeline* gpuShaderPipeline = nullptr;
    GPUShaderModule* vertexShader = nullptr;
    GPUShaderModule* fragmentShader = nullptr;
    GPUShaderDescriptorSets* mGPUShaderDescriptorSets;
    GPUVertexInputData mGPUVertexInputData;
public:
    GET(GPUShaderDescriptorSets)
};
REGISTER_CLASS(GPUShader);
