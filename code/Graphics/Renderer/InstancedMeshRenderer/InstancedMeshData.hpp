#pragma once

#include "Core/Minimal.hpp"
#include "GPU/Mesh/GPUMesh.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"

class InstancedMeshData
{
public:
	InstancedMeshData() = default;
	
	WeakPtr<Shader> mShader;
	WeakPtr<const GPUMesh> mMesh;
	bool mIsStatic = true;
    StencilData mStencilData;

	void init(TComponentHandler<MeshRenderer> renderer)
    {
        mShader = renderer->getRendererData().mShader;
        mMesh = renderer->getRendererData().mMesh;
        mIsStatic = renderer->isStatic();
        mStencilData = renderer->getRendererData().mStencilData;
    }

	bool operator==(const InstancedMeshData& otherInstancedMeshData) const
	{
        bool result = mShader == otherInstancedMeshData.mShader && mMesh == otherInstancedMeshData.mMesh and
        mIsStatic == otherInstancedMeshData.mIsStatic and
        mStencilData.matches(otherInstancedMeshData.mStencilData);
        return result;
	}

	class InstancedMeshDataFunctor
	{
	public:
		size_t operator()(const InstancedMeshData& key) const
		{
            u32 shift = 0;
            u64 result = key.mShader->getID() << (shift++);
            result = result ^ key.mMesh->mMeshID << (shift++);
			result = result ^ static_cast<u64>(key.mIsStatic) << (shift++);
            if(key.mStencilData.mUseStencil)
            {
                result = result ^ (key.mStencilData.hash() << (shift++));
            }
            
            return result;
		}
	};
};