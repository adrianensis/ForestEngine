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

    bool containsLayout(GPU::u64 key) const;
    const GPUDescriptorLayout& getLayout(GPU::u64 key) const;
    const GPUDescriptorLayout& addLayout(GPU::u64 key, GPUDescriptorLayoutData& gpuDescriptorLayoutData);

    bool containsPool(GPU::u64 key) const;
    const GPUDescriptorPool& getPool(GPU::u64 key) const;
    const GPUDescriptorPool& addPool(GPU::u64 key, const GPUDescriptorPoolData& gpuDescriptorPoolData);

    bool containsSet(GPU::u64 key) const;
    const GPUDescriptorSet& getSet(GPU::u64 key) const;
    GPUDescriptorSet& getSet(GPU::u64 key);
    const GPUDescriptorSet& addSet(GPU::u64 key, const GPUDescriptorPool& gpuDescriptorPool, GPUDescriptorLayoutData& gpuDescriptorLayoutData);
private:
    GPUContext* mGPUContext = nullptr;
    GPUTextureManager* mGPUTextureManager = nullptr;
    std::unordered_map<GPU::u64, GPUDescriptorLayout> mLayouts;
    std::unordered_map<GPU::u64, GPUDescriptorPool> mPools;
    std::unordered_map<GPU::u64, GPUDescriptorSet> mSets;
};

