#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Optimizations/Batch.hpp"
#include "Graphics/Renderer.hpp"

class Batch: public ObjectBase
{
    GENERATE_METADATA(Batch)

	PRI std::list<ProxyObject<Renderer>> mProxyRenderers;

	PRI Material* mMaterial = nullptr;
	PRI const Mesh* mMesh = nullptr;
	PRI bool mIsInstanced = false;
	PRI std::vector<f32> mMatrices;

	PRI Mesh mMeshBuilder;

	PRI u32 mVAO = 0;
	PRI u32 mVBOPosition = 0; // TODO: change u32 for GLuint
	PRI u32 mVBOTexture = 0;
	PRI u32 mVBOColor = 0;
	PRI u32 mVBOMatrices = 0;
	PRI u32 mEBO = 0;

	PRI u32 mMaxMeshesThreshold = 0;
	PRI const u32 mMaxMeshesIncrement = 100;
	PRI u32 mMeshesIndex = 0;

	PRI bool mBinded = false;
	PRI bool mDataSentToGPU = false;
	PRI bool mIsWorldSpace = false;
	PRI bool mIsStatic = false;

	PRI bool mNewRendererAdded = false;
	PRI bool mPendingDrawCall = false;
	PRI bool mForceRegenerateBuffers = false;

PRI
	bool shouldRegenerateBuffers() const;

	void addToVertexBuffer(Renderer& renderer);

	void generateFacesData(u32 meshesCount);
	void invalidateAndReallocateBuffers();
	void sendDataToBuffers();

	void addToVertexBufferNotInstanced(Renderer& renderer);
	void addToVertexBufferInstanced(Renderer& renderer);

	void resizeBuffers();
	bool isChunkOk(Renderer& renderer) const;

	bool processRenderers();

	void drawCall();

	void internalRemoveRendererFromList(std::list<ProxyObject<Renderer>>::iterator & it);
public:
	~Batch() override;

	void init(const Mesh *mesh, Material *material);
	void bind();

	void render();

	void addRenderer(Renderer& renderer);
	void forceRegenerateBuffers() { mForceRegenerateBuffers = true; }

	GET(Material)
	GET(Mesh)
	GET(IsInstanced)
	GET_SET(IsWorldSpace)
	GET_SET(IsStatic)
};