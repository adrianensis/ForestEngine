#ifndef BATCH_HPP
#define BATCH_HPP

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Batch/MeshBatcher.hpp"
#include "Graphics/Renderer/Renderer.hpp"

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

	void init(Ptr<Renderer> renderer)
    {
        mMaterial = renderer.get().getMaterial();
        mMesh = renderer.get().getMesh();
        mIsStatic = renderer.get().isStatic();
        mIsWorldSpace = renderer.get().getIsWorldSpace();
        mIsInstanced = renderer.get().getIsInstanced();
        mStencilValue = renderer.get().getStencilValue();
        mIsStencilMask = renderer.get().getIsStencilMask();
        mStencilFunction = renderer.get().getStencilFunction();
    }

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

class Batch: public ObjectBase
{
    GENERATE_METADATA(Batch)

public:
	~Batch() override;

    void init(const BatchData& batchData);

    void render();
    void addRenderer(Ptr<Renderer> renderer);
	void forceRegenerateBuffers() { mForceRegenerateBuffers = true; }

private:
    bool processRenderers();
    bool shouldRemoveRenderer(Ptr<Renderer> renderer);
    void internalRemoveRenderer(std::vector<Ptr<Renderer>>::iterator& it);
    void addToVertexBuffer(Ptr<Renderer> renderer);
    bool shouldRegenerateBuffers() const;

	bool isModelAnimated() const;

private:
	std::vector<Ptr<Renderer>> mRenderers;

	MeshBatcher mMeshBatcher;

    BatchData mBatchData;

	bool mNewRendererAdded = false;
	bool mPendingDrawCall = false;
	bool mForceRegenerateBuffers = false;
};

#endif