#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Batch/MeshBatcher.hpp"
#include "Graphics/Renderer/Renderer.hpp"

class BatchData
{
public:
	BatchData() = default;
	
	Ptr<const Material> mMaterial;
	Ptr<const Mesh> mMesh;
	bool mIsStatic = true;
	bool mIsWorldSpace = true;
	bool mIsInstanced = false;
	u32 mStencilValue = 0x00;
    u32 mStencilFunction = 0;
	bool mIsStencilMask = false;

	void init(Ptr<Renderer> renderer)
    {
        mMaterial = renderer->mMaterial;
        mMesh = renderer->mMesh;
        mIsStatic = renderer->isStatic();
        mIsWorldSpace = renderer->getIsWorldSpace();
        mIsInstanced = renderer->mIsInstanced;
        mStencilValue = renderer->mStencilValue;
        mIsStencilMask = renderer->mIsStencilMask;
        mStencilFunction = renderer->mStencilFunction;
    }

	bool operator==(const BatchData& otherBatchData) const
	{
        return mMaterial == otherBatchData.mMaterial and mMesh == otherBatchData.mMesh and
        mIsStatic == otherBatchData.mIsStatic and mIsWorldSpace == otherBatchData.mIsWorldSpace and mIsInstanced == otherBatchData.mIsInstanced and
        mStencilValue == otherBatchData.mStencilValue and mIsStencilMask == otherBatchData.mIsStencilMask and mStencilFunction == otherBatchData.mStencilFunction;
	}

	class BatchDataFunctor
	{
	public:
		size_t operator()(const BatchData& key) const
		{
			return key.mMaterial->getObjectId() ^ key.mMesh->getObjectId() ^
			static_cast<u64>(key.mIsStatic) ^ static_cast<u64>(key.mIsWorldSpace) ^ static_cast<u64>(key.mIsInstanced) ^
			(u64)key.mStencilValue ^ static_cast<u64>(key.mIsStencilMask) ^ static_cast<u64>(key.mStencilFunction);
		}
	};
};

class Batch: public ObjectBase
{
    GENERATE_METADATA(Batch)

public:
    void init(const BatchData& batchData);

    void render();
    void addRenderer(Ptr<Renderer> renderer);
	void forceRegenerateBuffers() { mForceRegenerateBuffers = true; }

private:
    void enable();
    void disable();
    void processRenderers();
    bool shouldRemoveRenderer(Ptr<Renderer> renderer);
    void internalRemoveRenderer(std::list<Ptr<Renderer>>::iterator& it);
    void addToVertexBuffer(Ptr<Renderer> renderer);
    bool shouldRegenerateBuffers() const;

private:
	std::list<Ptr<Renderer>> mRenderers;

    OwnerPtr<Shader> mShader;

	MeshBatcher mMeshBatcher;

    BatchData mBatchData;

	bool mNewRendererAdded = false;
	bool mForceRegenerateBuffers = false;
};
