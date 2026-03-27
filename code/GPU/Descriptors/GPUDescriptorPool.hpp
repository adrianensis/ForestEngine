#pragma once

#include "GPU/Core/GPUContext.hpp"

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

