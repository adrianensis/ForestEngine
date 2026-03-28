#pragma once

#include "GPU/Core/GPUContext.hpp"
#include "GPU/Descriptors/GPUDescriptorPool.hpp"
#include "GPU/Descriptors/GPUDescriptorSet.hpp"
#include "GPU/Descriptors/GPUDescriptorLayout.hpp"
#include "GPU/Texture/GPUTextureManager.hpp"

// TODO: Improve mapping, remove unordered map, use Slots manager
class GPUDescriptorManager
{    
public:
    void init(GPUContext* gpuContext, GPUTextureManager* gpuTextureManager);
    void terminate();

    bool containsLayout(Core::u64 key) const;
    const GPUDescriptorLayout& getLayout(Core::u64 key) const;
    const GPUDescriptorLayout& addLayout(Core::u64 key, GPUDescriptorLayoutData& gpuDescriptorLayoutData);

    bool containsPool(Core::u64 key) const;
    const GPUDescriptorPool& getPool(Core::u64 key) const;
    const GPUDescriptorPool& addPool(Core::u64 key, const GPUDescriptorPoolData& gpuDescriptorPoolData);

    bool containsSet(Core::u64 key) const;
    const GPUDescriptorSet& getSet(Core::u64 key) const;
    GPUDescriptorSet& getSet(Core::u64 key);
    const GPUDescriptorSet& addSet(Core::u64 key, const GPUDescriptorPool& gpuDescriptorPool, GPUDescriptorLayoutData& gpuDescriptorLayoutData);
private:
    GPUContext* mGPUContext = nullptr;
    GPUTextureManager* mGPUTextureManager = nullptr;
    std::unordered_map<Core::u64, GPUDescriptorLayout> mLayouts;
    std::unordered_map<Core::u64, GPUDescriptorPool> mPools;
    std::unordered_map<Core::u64, GPUDescriptorSet> mSets;
};

