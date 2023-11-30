#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"

class BatchData
{
public:
	BatchData() = default;
	
	Ptr<const Material> mMaterial;
	Ptr<const Mesh> mMesh;
	bool mIsStatic = true;
	bool mIsWorldSpace = true;
	bool mIsInstanced = false;
    StencilData mStencilData;

	void init(Ptr<MeshRenderer> renderer)
    {
        mMaterial = renderer->getRendererData().mMaterial;
        mMesh = renderer->getRendererData().mMesh;
        mIsStatic = renderer->isStatic();
        mIsWorldSpace = renderer->getIsWorldSpace();
        mIsInstanced = renderer->getRendererData().mIsInstanced;
        mStencilData = renderer->getRendererData().mStencilData;
    }

	bool operator==(const BatchData& otherBatchData) const
	{
        return mMaterial == otherBatchData.mMaterial and mMesh == otherBatchData.mMesh and
        mIsStatic == otherBatchData.mIsStatic and mIsWorldSpace == otherBatchData.mIsWorldSpace and mIsInstanced == otherBatchData.mIsInstanced and
        mStencilData == otherBatchData.mStencilData;
	}

	class BatchDataFunctor
	{
	public:
		size_t operator()(const BatchData& key) const
		{
			return key.mMaterial->getObjectId() ^ key.mMesh->getObjectId() ^
			static_cast<u64>(key.mIsStatic) ^ static_cast<u64>(key.mIsWorldSpace) ^ static_cast<u64>(key.mIsInstanced) ^
            (u64)key.mStencilData.mUseStencil ^ 
			(u64)key.mStencilData.mMaskStencilValue ^ static_cast<u64>(key.mStencilData.mStencilFunction) ^
            (u64)key.mStencilData.mMaskObjectId ^ (u64)key.mStencilData.mThisObjectId;
		}
	};
};