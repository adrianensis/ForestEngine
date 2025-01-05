#pragma once

#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUContext.hpp"

class GPUShaderModule {

public:
    explicit GPUShaderModule(Ptr<GPUContext> gpuContext);
    bool init(const std::vector<byte>& moduleContent);
    void terminate();
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;
    Ptr<GPUContext> mGPUContext;
    VkShaderModule mShaderModule = VK_NULL_HANDLE;
public:
    CRGET(ShaderModule)
};