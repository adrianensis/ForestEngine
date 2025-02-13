#pragma once

#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Shader/GPUShaderCompiler.hpp"

class GPUShaderModuleData
{
public:
    GPUPipelineStage mStage;
    std::string mModuleContent;
    HashedString id;
};

class GPUShaderModule
{
public:
    bool init(Ptr<GPUContext> gpuContext, const GPUShaderModuleData& gpuGPUShaderModuleData);
    void terminate();
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;
    Ptr<GPUContext> mGPUContext;
    VkShaderModule mGPUShaderModule = VK_NULL_HANDLE;
    HashedString mID;
public:
    CRGET(GPUShaderModule)
};