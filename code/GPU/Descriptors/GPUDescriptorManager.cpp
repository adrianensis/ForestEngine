#include "GPU/Descriptors/GPUDescriptorManager.hpp"
#include "GPU/Descriptors/GPUDescriptorLayout.hpp"
#include "GPU/Descriptors/GPUDescriptorPool.hpp"
#include <utility>

void GPUDescriptorManager::init(GPUContext* gpuContext)
{
    mGPUContext = gpuContext;


}

void GPUDescriptorManager::terminate()
{
}

bool GPUDescriptorManager::containsLayout(Core::u64 key) const
{
    return mLayout.contains(key);
}

const GPUDescriptorLayout& GPUDescriptorManager::getLayout(Core::u64 key) const
{
    return mLayout.at(key);
}

const GPUDescriptorLayout& GPUDescriptorManager::addLayout(Core::u64 key, GPUDescriptorLayoutData& gpuDescriptorLayoutData)
{
    if(containsLayout(key))
    {
        return getLayout(key);
    }
    mLayout.insert(std::make_pair(key, GPUDescriptorLayout{}));

    GPUDescriptorLayout& gpuDescriptorLayout = mLayout[key];
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
    return getPool(key);
}

bool GPUDescriptorManager::containsSet(Core::u64 key) const
{
    return mSets.contains(key);
}

const GPUDescriptorSet& GPUDescriptorManager::getSet(Core::u64 key) const
{
    return mSets.at(key);
}

const GPUDescriptorSet& GPUDescriptorManager::addSet(Core::u64 key)
{
    if(containsSet(key))
    {
        return getSet(key);
    }
    mSets.insert(std::make_pair(key, GPUDescriptorSet{}));
    return getSet(key);
}
