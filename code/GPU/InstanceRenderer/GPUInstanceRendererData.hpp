#pragma once

#include "GPU/RenderItem/GPURenderItem.hpp"

class GPUInstanceRendererData
{
public:
	GPUInstanceRendererData() = default;
	
	Core::WeakPtr<GPUShader> mShader;
	Core::WeakPtr<const GPUMesh> mMesh;
	bool mIsStatic = true;
    GPUDepthStencilData mGPUDepthStencilData;

	void init(Core::WeakPtr<GPURenderItem> renderItem)
    {
        mShader = renderItem->getGPURenderItemData().mShader;
        mMesh = renderItem->getGPURenderItemData().mMesh;
        mIsStatic = renderItem->isStatic();
        mGPUDepthStencilData = renderItem->getGPURenderItemData().mGPUDepthStencilData;
    }

	bool operator==(const GPUInstanceRendererData& otherGPUInstanceRendererData) const
	{
        bool result = mShader == otherGPUInstanceRendererData.mShader and
        mMesh == otherGPUInstanceRendererData.mMesh and
        mIsStatic == otherGPUInstanceRendererData.mIsStatic and
        mGPUDepthStencilData == otherGPUInstanceRendererData.mGPUDepthStencilData;
        return result;
	}

	class GPUInstanceRendererDataFunctor
	{
	public:
		size_t operator()(const GPUInstanceRendererData& key) const
		{
            Core::u32 shift = 0;
            Core::u64 result = key.mShader->getID() << (shift++);
            result = result ^ key.mMesh->mMeshID << (shift++);
			result = result ^ static_cast<Core::u64>(key.mIsStatic) << (shift++);
            if(key.mGPUDepthStencilData.mStencilEnable)
            {
                result = result ^ (key.mGPUDepthStencilData.hash() << (shift++));
            }
            
            return result;
		}
	};
};