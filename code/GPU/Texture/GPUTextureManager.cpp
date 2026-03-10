#include "GPU/Texture/GPUTextureManager.hpp"

void GPUTextureManager::init()
{
    mTextures.reserve(mInitialTextures);
    // INFO: We reserve position 0 to represent NULL
    mTextures.emplace_back();
}

void GPUTextureManager::terminate()
{
    FOR_LIST(it, mTextures)
    {
        if(*it)
        {
            (*it)->terminate();
        }
    }
}

Core::WeakPtr<GPUTexture> GPUTextureManager::loadTexture(GPUContext* gpuContext, const GPUTextureData& gpuTextureData)
{
	if (!mTexturesByPath.contains(gpuTextureData.mPath))
	{
        PROFILER_CPU()
        Core::WeakPtr<GPUTexture> texture = mTextures.emplace_back(Core::OwnerPtr<GPUTexture>::newObject());
        mTexturesByPath.insert_or_assign(gpuTextureData.mPath, texture);
        texture->init(gpuContext, gpuTextureData, mTextures.size() - 1);
	}

	return mTexturesByPath.at(gpuTextureData.mPath);
}