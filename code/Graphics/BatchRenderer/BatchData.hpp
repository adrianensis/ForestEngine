#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"

class BatchData
{
public:
	BatchData() = default;
	
	u32 mMaterialId;
	Ptr<const Mesh> mMesh;
	bool mIsStatic = true;
	bool mIsWorldSpace = true;
	bool mIsInstanced = false;
    StencilData mStencilData;

	void init(Ptr<MeshRenderer> renderer)
    {
        mMaterialId = renderer->getRendererData().mMaterialId;
        mMesh = renderer->getRendererData().mMesh;
        mIsStatic = renderer->isStatic();
        mIsWorldSpace = renderer->getIsWorldSpace();
        mIsInstanced = renderer->getRendererData().mIsInstanced;
        mStencilData = renderer->getRendererData().mStencilData;
    }

	bool operator==(const BatchData& otherBatchData) const
	{
        return mMaterialId == otherBatchData.mMaterialId && mMesh == otherBatchData.mMesh and
        mIsStatic == otherBatchData.mIsStatic && mIsWorldSpace == otherBatchData.mIsWorldSpace && mIsInstanced == otherBatchData.mIsInstanced and
        mStencilData == otherBatchData.mStencilData;
	}

	class BatchDataFunctor
	{
	public:
		size_t operator()(const BatchData& key) const
		{
			return key.mMaterialId ^ key.mMesh->getObjectId() ^
			static_cast<u64>(key.mIsStatic) ^ static_cast<u64>(key.mIsWorldSpace) ^ static_cast<u64>(key.mIsInstanced) ^
            (u64)key.mStencilData.mUseStencil ^ 
			(u64)key.mStencilData.mStencilValue ^ static_cast<u64>(key.mStencilData.mStencilFunction) ^
            (u64)key.mStencilData.mParentId ^ (u64)key.mStencilData.mId;
		}
	};
};