#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Buffers/VertexBuffer.hpp"
#include "Graphics/Renderer.hpp"

class Batch: public ObjectBase
{
    GENERATE_METADATA(Batch)

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

	bool shouldRegenerateBuffers() const;

	void addToVertexBuffer(Ptr<Renderer> renderer);

	bool isChunkOk(Ptr<Renderer> renderer) const;

	bool processRenderers();

	void internalRemoveRendererFromList(std::list<Ptr<Renderer>>::iterator & it);
public:
	~Batch() override;

	void init(Ptr<const Mesh> mesh, Ptr<Material> material, bool isStatic, bool isWorldSpace);

	void render();

	void addRenderer(Ptr<Renderer> renderer);
	void forceRegenerateBuffers() { mForceRegenerateBuffers = true; }

	RGET(Material)
	GET(IsInstanced)
	GET(IsWorldSpace)
	GET(IsStatic)
};