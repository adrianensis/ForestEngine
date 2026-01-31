#pragma once

#include "GPU/Core/GPUContext.hpp"
#include "GPU/Descriptors/GPUDescriptorPool.hpp"
#include "GPU/Descriptors/GPUDescriptorSet.hpp"
#include "GPU/Descriptors/GPUDescriptorLayout.hpp"

// TODO: Refactor, separate Layout and Pool, for bindless architecture
class GPUDescriptorManager
{    
public:
    void init(GPUContext* gpuContext);
    void terminate();

    bool containsLayout(Core::u64 key) const;
    const GPUDescriptorLayout& getLayout(Core::u64 key) const;
    const GPUDescriptorLayout& addLayout(Core::u64 key, GPUDescriptorLayoutData& gpuDescriptorLayoutData);

    bool containsPool(Core::u64 key) const;
    const GPUDescriptorPool& getPool(Core::u64 key) const;
    const GPUDescriptorPool& addPool(Core::u64 key);

    bool containsSet(Core::u64 key) const;
    const GPUDescriptorSet& getSet(Core::u64 key) const;
    const GPUDescriptorSet& addSet(Core::u64 key);
private:
    GPUContext* mGPUContext = nullptr;
    std::unordered_map<Core::u64, GPUDescriptorLayout> mLayout;
    std::unordered_map<Core::u64, GPUDescriptorPool> mPools;
    std::unordered_map<Core::u64, GPUDescriptorSet> mSets;
};

