#pragma once

#include "Graphics/GPU/Core/GPUDevice.h"
#include "Graphics/GPU/Core/GPUContext.hpp"

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