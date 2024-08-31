#pragma once

#include "Graphics/GPU/GPUDevice.h"

class GPUProgramModule {
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

private:
    GPUDevice* vulkanDevice;
    VkShaderModule shaderModule = VK_NULL_HANDLE;

public:
    explicit GPUProgramModule(GPUDevice* vulkanDevice);
    const VkShaderModule getShaderModule() const;
    bool initialize(const std::vector<byte>& moduleContent);
    void terminate();
};