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
        Core::Slot slot = mTextureSlotManager.requestSlot();
        mTexturesByPath.insert_or_assign(gpuTextureData.mPath, slot);
        mTextures[slot.getSlot()] = Core::OwnerPtr<GPUTexture>::newObject();
        Core::WeakPtr<GPUTexture> texture = mTextures[slot.getSlot()];

        texture->init(gpuContext, gpuTextureData, slot);
	}

    Core::Slot slot = mTexturesByPath.at(gpuTextureData.mPath);
	return mTextures[slot.getSlot()];
}