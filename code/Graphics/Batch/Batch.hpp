#ifndef BATCH_HPP
#define BATCH_HPP

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Buffers/VertexBuffer.hpp"
#include "Graphics/Renderer/Renderer.hpp"

class BatchKey
{
public:
	BatchKey() = default;
	
	Ptr<Material> mMaterial;
	Ptr<const Mesh> mMesh;
	bool mIsStatic = true;
	bool mIsWorldSpace = true;
	bool mIsInstanced = false;
	u32 mStencilValue = 0x00;
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
	}

	bool operator==(const BatchKey& otherBatchKey) const
	{
		return mMaterial == otherBatchKey.mMaterial && mMesh == otherBatchKey.mMesh &&
			mIsStatic == otherBatchKey.mIsStatic && mIsWorldSpace == otherBatchKey.mIsWorldSpace && mIsInstanced == otherBatchKey.mIsInstanced &&
			mStencilValue == otherBatchKey.mStencilValue && mIsStencilMask == otherBatchKey.mIsStencilMask;
	}

	class BatchKeyFunctor
	{
	public:
	
		// Use sum of lengths of first and last names
		// as hash function.
		size_t operator()(const BatchKey& key) const
		{
			return key.mMaterial.get().getHash() ^ key.mMesh.get().getHash() ^
			static_cast<u64>(key.mIsStatic) ^ static_cast<u64>(key.mIsWorldSpace) ^ static_cast<u64>(key.mIsInstanced) ^
			(u64)key.mStencilValue ^ static_cast<u64>(key.mIsStencilMask);
		}
	};
};

class Batch: public ObjectBase
{
    GENERATE_METADATA(Batch)

public:
	~Batch() override;

    void init(const BatchKey& batchKey);

    void render();
    void addRenderer(Ptr<Renderer> renderer);
	void forceRegenerateBuffers() { mForceRegenerateBuffers = true; }

private:
    bool processRenderers();
    bool shouldRemoveRenderer(Ptr<Renderer> renderer);
    void internalRemoveRendererFromList(std::list<Ptr<Renderer>>::iterator& it);
    void addToVertexBuffer(Ptr<Renderer> renderer);
    bool shouldRegenerateBuffers() const;

	bool isModelAnimated() const;

	void enableStencil() const;
	void disableStencil() const;

private:
	std::list<Ptr<Renderer>> mRenderers;

	Ptr<Material> mMaterial;

	MeshBatcher mMeshBatcher;

	bool mIsWorldSpace = false;
	bool mIsStatic = false;
	bool mIsInstanced = false;
	u32 mStencilValue = 0x00;
	bool mIsStencilMask = false;

	bool mNewRendererAdded = false;
	bool mPendingDrawCall = false;
	bool mForceRegenerateBuffers = false;

public:
	RGET(Material)
	GET(IsInstanced)
	GET(IsWorldSpace)
	GET(IsStatic)
};

#endif