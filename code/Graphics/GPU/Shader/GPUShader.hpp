#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/GPU/Buffer/GPUUniformBuffer.hpp"
#include "Graphics/GPU/Buffer/GPUVertexBuffer.hpp"
#include "Graphics/GPU/Core/GPUContext.hpp"
#include "Graphics/GPU/Shader/GPUShaderDescriptorSets.hpp"
#include "Core/Object/ObjectBase.hpp"
#include "Graphics/GPU/Shader/GPUShaderPipeline.h"

class GPURenderPass;

class GPUShader: public ObjectBase
{    
public:

    GPUShader();
	~GPUShader() override = default;

    void initFromFileContents(GPURenderPass* vulkanRenderPass, GPUShaderDescriptorSets* gpuShaderDescriptorSets, const std::vector<GPUVertexBuffer>& vertexInputBuffers, Ptr<GPUContext> gpuContext, const std::vector<byte>& vertex, const std::vector<byte>& fragment);
    void terminate();
    void enable() const;
    void disable() const;
private:
	u32 mProgramId = 0;

    Ptr<GPUContext> mGPUContext;

    GPUShaderPipeline* gpuShaderPipeline;

    GPUShaderModule* vertexShader;
    GPUShaderModule* fragmentShader;
    GPUShaderDescriptorSets* mGPUDescriptor;
};
REGISTER_CLASS(GPUShader);
