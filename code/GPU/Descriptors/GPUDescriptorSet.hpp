#pragma once

#include "GPU/Core/GPUContext.hpp"
#include "GPU/Descriptors/GPUDescriptorLayout.hpp"

class GPUDescriptorSetBindings
{
public:
    std::unordered_map<Core::HashedString, Core::u32> mBindings;
    std::unordered_map<Core::HashedString, Core::u32> mSets;
};

// TODO: Refactor, separate Layout and Pool, for bindless architecture
class GPUDescriptorSet
{    
public:
    void init(const GPUDescriptorLayoutData& gpuDescriptorLayoutData, GPUContext* gpuContext);
    void update();
    void terminate();
private:
    GPUContext* mGPUContext = nullptr;
public:
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    GPUDescriptorLayout mGPUDescriptorLayout;
    GPUDescriptorSetBindings mGPUDescriptorSetBindings;
    Core::u32 mSamplersBindingIndexOffset = 0;
};

