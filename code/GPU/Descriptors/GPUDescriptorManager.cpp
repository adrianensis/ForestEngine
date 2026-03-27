#include "GPU/Descriptors/GPUDescriptorManager.hpp"
#include "GPU/Descriptors/GPUDescriptorLayout.hpp"
#include "GPU/Descriptors/GPUDescriptorPool.hpp"
#include <utility>

void GPUDescriptorManager::init(GPUContext* gpuContext, GPUTextureManager* gpuTextureManager)
{
    mGPUContext = gpuContext;
    mGPUTextureManager = gpuTextureManager;
}

void GPUDescriptorManager::terminate()
{
}

bool GPUDescriptorManager::containsLayout(Core::u64 key) const
{
    return mLayouts.contains(key);
}

const GPUDescriptorLayout& GPUDescriptorManager::getLayout(Core::u64 key) const
{
    return mLayouts.at(key);
}

const GPUDescriptorLayout& GPUDescriptorManager::addLayout(Core::u64 key, GPUDescriptorLayoutData& gpuDescriptorLayoutData)
{
    if(containsLayout(key))
    {
        return getLayout(key);
    }
    mLayouts.insert(std::make_pair(key, GPUDescriptorLayout{}));

    GPUDescriptorLayout& gpuDescriptorLayout = mLayouts[key];
    gpuDescriptorLayout.init(gpuDescriptorLayoutData, mGPUContext);
    return gpuDescriptorLayout;
}

bool GPUDescriptorManager::containsPool(Core::u64 key) const
{
    return mPools.contains(key);
}

const GPUDescriptorPool& GPUDescriptorManager::getPool(Core::u64 key) const
{
    return mPools.at(key);
}

const GPUDescriptorPool& GPUDescriptorManager::addPool(Core::u64 key)
{
    if(containsPool(key))
    {
        return getPool(key);
    }
    mPools.insert(std::make_pair(key, GPUDescriptorPool{}));

    GPUDescriptorPool& gpuDescriptorPool = mPools[key];
    gpuDescriptorPool.init(mGPUContext);
    return gpuDescriptorPool;
}

bool GPUDescriptorManager::containsSet(Core::u64 key) const
{
    return mSets.contains(key);
}

GPUDescriptorSet& GPUDescriptorManager::getSet(Core::u64 key)
{
    return mSets.at(key);
}

const GPUDescriptorSet& GPUDescriptorManager::getSet(Core::u64 key) const
{
    return mSets.at(key);
}

const GPUDescriptorSet& GPUDescriptorManager::addSet(Core::u64 key, const GPUDescriptorPool& gpuDescriptorPool, GPUDescriptorLayoutData& gpuDescriptorLayoutData)
{
    if(containsSet(key))
    {
        return getSet(key);
    }
    mSets.insert(std::make_pair(key, GPUDescriptorSet{}));

    GPUDescriptorSet& gpuDescriptorSet = mSets[key];
    gpuDescriptorSet.init(gpuDescriptorLayoutData, gpuDescriptorPool, mGPUTextureManager, mGPUContext);
    return gpuDescriptorSet;
}
