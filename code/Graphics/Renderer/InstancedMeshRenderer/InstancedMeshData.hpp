#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/GPU/Mesh/GPUMesh.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"

class InstancedMeshData
{
public:
	InstancedMeshData() = default;
	
	PoolHandler<Material> mMaterial;
	Ptr<const GPUMesh> mMesh;
	bool mIsStatic = true;
    StencilData mStencilData;

	void init(TypedComponentHandler<MeshRenderer> renderer)
    {
        mMaterial = renderer->getRendererData().mMaterial;
        mMesh = renderer->getRendererData().mMesh;
        mIsStatic = renderer->isStatic();
        mStencilData = renderer->getRendererData().mStencilData;
    }

	bool operator==(const InstancedMeshData& otherInstancedMeshData) const
	{
        bool result = mMaterial.getIndex() == otherInstancedMeshData.mMaterial.getIndex() && mMesh == otherInstancedMeshData.mMesh and
        mIsStatic == otherInstancedMeshData.mIsStatic and
        mStencilData.matches(otherInstancedMeshData.mStencilData);
        return result;
	}

	class InstancedMeshDataFunctor
	{
	public:
		size_t operator()(const InstancedMeshData& key) const
		{
            u64 result = key.mMaterial.getIndex() ^ key.mMesh->getObjectId() ^
			static_cast<u64>(key.mIsStatic);
            if(key.mStencilData.mUseStencil)
            {
                result = result ^ key.mStencilData.hash();
            }
            
            return result;
		}
	};
};