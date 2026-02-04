#pragma once

#include "GPU/Core/GPUContext.hpp"
#include "GPU/Descriptors/GPUDescriptorLayout.hpp"
#include "GPU/Descriptors/GPUDescriptorPool.hpp"

// TODO: Refactor, separate Layout and Pool, for bindless architecture
class GPUDescriptorSet
{    
public:
    void init(const GPUDescriptorLayoutData& gpuDescriptorLayoutData, const GPUDescriptorPool& gpuDescriptorPool, GPUContext* gpuContext);
    void update();
    void terminate();
private:
    GPUContext* mGPUContext = nullptr;
public:
    std::vector<VkDescriptorSet> descriptorSets;
    GPUDescriptorLayout mGPUDescriptorLayout;
};

