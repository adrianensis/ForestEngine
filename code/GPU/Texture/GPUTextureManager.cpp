#include "GPU/Texture/GPUTextureManager.hpp"
#include "Core/Memory/SlotsManager.hpp"

void GPUTextureManager::init()
{
    mTextures.resize(mInitialTextures);
    mTextureSlotManager.init(mInitialTextures);
}

void GPUTextureManager::terminate()
{
    FOR_LIST(it, mTextures)
    {
        if((*it).isValid())
        {
            (*it).terminate();
        }
    }
}

GPUTextureHandle GPUTextureManager::loadTexture(GPUContext* gpuContext, const GPUTextureData& gpuTextureData)
{
    PROFILER_CPU()
    GPUTextureHandle handle;
	if (mTexturesByPath.contains(gpuTextureData.mPath))
	{
        handle.mSlot = mTexturesByPath.at(gpuTextureData.mPath);
    }
    else
    {
        handle.mSlot = mTextureSlotManager.requestSlot();
        mTexturesByPath.insert_or_assign(gpuTextureData.mPath, handle.mSlot);
        GPUTexture& texture = mTextures[handle.mSlot.getSlot()];

        texture.init(gpuContext, gpuTextureData, handle.mSlot);
	}

#ifdef ENGINE_BUILD_DEBUG
    handle.mDebugGPUTexture = &mTextures[handle.mSlot.getSlot()];
#endif

	return handle;
}

const GPUTexture& GPUTextureManager::getTexture(const GPUTextureHandle& handle) const
{
    return mTextures[handle.mSlot.getSlot()];
}