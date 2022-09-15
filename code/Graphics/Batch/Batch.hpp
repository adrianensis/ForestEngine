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

	void init(Ptr<Renderer> renderer)
	{
		mMaterial = renderer.get().getMaterial();
		mMesh = renderer.get().getMesh();
		mIsStatic = renderer.get().isStatic();
		mIsWorldSpace = renderer.get().getIsWorldSpace();
		mIsInstanced = renderer.get().getIsInstanced();
	}

	bool operator==(const BatchKey& otherBatchKey) const
	{
		return mMaterial == otherBatchKey.mMaterial && mMesh == otherBatchKey.mMesh &&
			mIsStatic == otherBatchKey.mIsStatic && mIsWorldSpace == otherBatchKey.mIsWorldSpace && mIsInstanced == otherBatchKey.mIsInstanced;
	}

	class BatchKeyFunctor
	{
	public:
	
		// Use sum of lengths of first and last names
		// as hash function.
		size_t operator()(const BatchKey& key) const
		{
			u64 materialHash = key.mMaterial.isValid() ? key.mMaterial.get().getHash() : 0;
			return materialHash ^ key.mMesh.get().getHash() ^
			static_cast<u64>(key.mIsStatic) ^ static_cast<u64>(key.mIsWorldSpace) ^ static_cast<u64>(key.mIsInstanced);
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

private:
	std::list<Ptr<Renderer>> mRenderers;

	Ptr<Material> mMaterial;

	MeshBatcher mMeshBatcher;

	bool mIsWorldSpace = false;
	bool mIsStatic = false;
	bool mIsInstanced = false;

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