#ifndef BATCH_HPP
#define BATCH_HPP

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Buffers/VertexBuffer.hpp"
#include "Graphics/Renderer.hpp"


class Batch: public ObjectBase
{
    GENERATE_METADATA(Batch)

public:
	~Batch() override
	{

		mRenderers.clear();
		mMeshBatcher.terminate();
	}

    void init(Ptr<const Mesh> mesh, Ptr<Material> material, bool isStatic, bool isWorldSpace);

    void render();
    void addRenderer(Ptr<Renderer> renderer);
	void forceRegenerateBuffers() { mForceRegenerateBuffers = true; }

private:
    bool processRenderers();
    bool shouldRemoveRenderer(Ptr<Renderer> renderer);
    void internalRemoveRendererFromList(std::list<Ptr<Renderer>>::iterator& it);
    void addToVertexBuffer(Ptr<Renderer> renderer);
    bool shouldRegenerateBuffers() const;

private:
	std::list<Ptr<Renderer>> mRenderers;

	Ptr<Material> mMaterial;
	bool mIsInstanced = false;

	MeshBatcher mMeshBatcher;

	bool mIsWorldSpace = false;
	bool mIsStatic = false;

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