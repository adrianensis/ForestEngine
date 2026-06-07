#include "GPU/Texture/GPUTextureManager.hpp"
#include "Core/CoreMacros.hpp"
#include "Core/Memory/SlotsManager.hpp"

void GPUTextureManager::init(GPUContext* gpuContext)
{
    mGPUContext = gpuContext;
    mTextures.resize(mInitialTextures);
}

void GPUTextureManager::terminate()
{
    for(auto& it: mTextures)
    {
        if(it.isValid())
        {
            it.terminate();
        }
    }
}

GPUTextureHandle GPUTextureManager::loadTexture(const GPUTextureData& gpuTextureData)
{
    PROFILER_CPU()
    GPUTextureHandle handle;
	if (mTexturesByPath.contains(gpuTextureData.mPath))
	{
        handle.mSlot = mTexturesByPath.at(gpuTextureData.mPath);
        handle.mIsValid = true;
    }
    else
    {

        FOR_RANGE(i, 0, mTextures.size())
        {
            if(!mTextures[i].isValid())
            {
                handle.mSlot = i;
                handle.mIsValid = true;
                break;
            }
        }

        mTexturesByPath.insert_or_assign(gpuTextureData.mPath, handle.mSlot);
        GPUTexture& texture = mTextures[handle.mSlot];

        texture.init(mGPUContext, gpuTextureData);
	}

    if(handle.mIsValid)
    {
        #ifdef ENGINE_BUILD_DEBUG
            handle.mDebugGPUTexture = &mTextures[handle.mSlot];
        #endif
    }

	return handle;
}

const GPUTexture& GPUTextureManager::getTexture(const GPUTextureHandle& handle) const
{
    return mTextures[handle.mSlot];
}