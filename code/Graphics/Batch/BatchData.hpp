#ifndef BATCHDATA_HPP
#define BATCHDATA_HPP

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Batch/MeshBatcher.hpp"

class Renderer;

class BatchData
{
public:
	BatchData() = default;
	
	Ptr<Material> mMaterial;
	Ptr<const Mesh> mMesh;
	bool mIsStatic = true;
	bool mIsWorldSpace = true;
	bool mIsInstanced = false;
	u32 mStencilValue = 0x00;
    u32 mStencilFunction = 0;
	bool mIsStencilMask = false;

	void init(Ptr<Renderer> renderer);

	bool operator==(const BatchData& otherBatchData) const
	{
        return mMaterial == otherBatchData.mMaterial && mMesh == otherBatchData.mMesh &&
        mIsStatic == otherBatchData.mIsStatic && mIsWorldSpace == otherBatchData.mIsWorldSpace && mIsInstanced == otherBatchData.mIsInstanced &&
        mStencilValue == otherBatchData.mStencilValue && mIsStencilMask == otherBatchData.mIsStencilMask && mStencilFunction == otherBatchData.mStencilFunction;
	}

	class BatchDataFunctor
	{
	public:
	
		// Use sum of lengths of first and last names
		// as hash function.
		size_t operator()(const BatchData& key) const
		{
			return key.mMaterial.get().getHash() ^ key.mMesh.get().getHash() ^
			static_cast<u64>(key.mIsStatic) ^ static_cast<u64>(key.mIsWorldSpace) ^ static_cast<u64>(key.mIsInstanced) ^
			(u64)key.mStencilValue ^ static_cast<u64>(key.mIsStencilMask) ^ static_cast<u64>(key.mStencilFunction);
		}
	};
};

#endif