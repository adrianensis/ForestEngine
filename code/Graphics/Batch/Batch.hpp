#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Batch/MeshBatcher.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/Shaders/Shader.hpp"

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
        mMaterial = renderer->getComponentData().mMaterial;
        mMesh = renderer->getComponentData().mMesh;
        mIsStatic = renderer->isStatic();
        mIsWorldSpace = renderer->getIsWorldSpace();
        mIsInstanced = renderer->getComponentData().mIsInstanced;
        mStencilData = renderer->getComponentData().mStencilData;
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
			(u64)key.mStencilData.mStencilValue ^ static_cast<u64>(key.mStencilData.mStencilFunction) ^
            (u64)key.mStencilData.mMaskObjectId ^ (u64)key.mStencilData.mThisObjectId;
		}
	};
};

class Batch: public ObjectBase
{
    GENERATE_METADATA(Batch)

public:
    void init(const BatchData& batchData);

    void render();
    void addRenderer(Ptr<MeshRenderer> renderer);
	void forceRegenerateBuffers() { mForceRegenerateBuffers = true; }

private:
    void enable();
    void disable();
    void processRenderers();
    bool shouldRemoveRenderer(Ptr<const MeshRenderer> renderer);
    void internalRemoveRenderer(std::list<Ptr<MeshRenderer>>::iterator& it);
    void addToVertexBuffer(Ptr<MeshRenderer> renderer);
    bool shouldRegenerateBuffers() const;

private:
	std::list<Ptr<MeshRenderer>> mRenderers;
    OwnerPtr<Shader> mShader;
	MeshBatcher mMeshBatcher;
    BatchData mBatchData;

	bool mNewRendererAdded = false;
	bool mForceRegenerateBuffers = false;
    bool mRenderStencilAsMask = false;

public:
    SET(RenderStencilAsMask)
};
