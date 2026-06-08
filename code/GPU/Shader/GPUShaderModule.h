#pragma once

#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Shader/GPUShaderCompiler.hpp"

class GPUShaderModuleData
{
public:
    GPUPipelineStage mStage;
    std::string mModuleContent;
    std::string id;
};

class GPUShaderModule
{
public:
    bool init(GPUContext* gpuContext, const GPUShaderModuleData& gpuShaderModuleData);
    void terminate();
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;
    GPUContext* mGPUContext = nullptr;
    VkShaderModule mGPUShaderModule = VK_NULL_HANDLE;
    std::string mID;
public:
    const auto& getGPUShaderModule() const { return mGPUShaderModule; }
};