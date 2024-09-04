#pragma once

#include "Graphics/GPU/GPUDevice.h"
#include "Graphics/GPU/GPUContext.hpp"

class GPUShaderModule {
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

private:
    Ptr<GPUContext> mGPUContext;
    VkShaderModule shaderModule = VK_NULL_HANDLE;

public:
    explicit GPUShaderModule(Ptr<GPUContext> gpuContext);
    const VkShaderModule getShaderModule() const;
    bool initialize(const std::vector<byte>& moduleContent);
    void terminate();
};