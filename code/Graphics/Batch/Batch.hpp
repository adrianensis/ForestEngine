#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Buffers/VertexBuffer.hpp"
#include "Graphics/Renderer.hpp"

class Batch: public ObjectBase
{
    GENERATE_METADATA(Batch)

private:
	std::list<Ref<Renderer>> mRenderers;

	Ref<Material> mMaterial;
	bool mIsInstanced = false;

	MeshBatcher mMeshBatcher;

	bool mIsWorldSpace = false;
	bool mIsStatic = false;

	bool mNewRendererAdded = false;
	bool mPendingDrawCall = false;
	bool mForceRegenerateBuffers = false;

	bool shouldRegenerateBuffers() const;

	void addToVertexBuffer(Ref<Renderer> renderer);

	bool isChunkOk(Ref<Renderer> renderer) const;

	bool processRenderers();

	void internalRemoveRendererFromList(std::list<Ref<Renderer>>::iterator & it);
public:
	~Batch() override;

	void init(Ref<const Mesh> mesh, Ref<Material> material, bool isStatic, bool isWorldSpace);

	void render();

	void addRenderer(Ref<Renderer> renderer);
	void forceRegenerateBuffers() { mForceRegenerateBuffers = true; }

	RGET(Material)
	GET(IsInstanced)
	GET(IsWorldSpace)
	GET(IsStatic)
};