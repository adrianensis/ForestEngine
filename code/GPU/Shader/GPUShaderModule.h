#pragma once

#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUContext.hpp"

class GPUShaderModule
{
public:
    bool init(WeakPtr<GPUContext> gpuContext, const std::vector<byte>& moduleContent);
    void terminate();
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;
    WeakPtr<GPUContext> mGPUContext;
    VkShaderModule mShaderModule = VK_NULL_HANDLE;
public:
    CRGET(ShaderModule)
};