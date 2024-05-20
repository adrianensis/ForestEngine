#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"

class BatchData
{
public:
	BatchData() = default;
	
	PoolHandler<Material> mMaterial;
	Ptr<const Mesh> mMesh;
	bool mIsStatic = true;
	bool mIsInstanced = false;
    StencilData mStencilData;

	void init(Ptr<MeshRenderer> renderer)
    {
        mMaterial = renderer->getRendererData().mMaterial;
        mMesh = renderer->getRendererData().mMesh;
        mIsStatic = renderer->isStatic();
        mIsInstanced = renderer->getRendererData().mIsInstanced;
        mStencilData = renderer->getRendererData().mStencilData;
    }

	bool operator==(const BatchData& otherBatchData) const
	{
        bool result = mMaterial.getIndex() == otherBatchData.mMaterial.getIndex() && mMesh == otherBatchData.mMesh and
        mIsStatic == otherBatchData.mIsStatic && mIsInstanced == otherBatchData.mIsInstanced and
        mStencilData.matches(otherBatchData.mStencilData);
        return result;
	}

	class BatchDataFunctor
	{
	public:
		size_t operator()(const BatchData& key) const
		{
            u64 result = key.mMaterial.getIndex() ^ key.mMesh->getObjectId() ^
			static_cast<u64>(key.mIsStatic) ^ static_cast<u64>(key.mIsInstanced);
            if(key.mStencilData.mUseStencil)
            {
                result = result ^ key.mStencilData.hash();
            }
            
            return result;
		}
	};
};