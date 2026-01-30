#pragma once

#include "GPU/Core/GPUContext.hpp"

// TODO: Refactor, separate Layout and Pool, for bindless architecture
class GPUDescriptorPool
{    
public:
    void init(GPUContext* gpuContext);
    void terminate();
private:
    GPUContext* mGPUContext = nullptr;
public:
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
};

