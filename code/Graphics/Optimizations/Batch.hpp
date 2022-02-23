#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Optimizations/Batch.hpp"
#include "Graphics/Renderer.hpp"

class Batch: public ObjectBase
{
    GENERATE_METADATA(Batch)

private:
	std::list<ProxyObject<Renderer>> mProxyRenderers;

	Material* mMaterial = nullptr;
	const Mesh* mMesh = nullptr;
	bool mIsInstanced = false;
	std::vector<f32> mMatrices;

	Mesh mMeshBuilder;

	u32 mVAO = 0;
	u32 mVBOPosition = 0; // TODO: change u32 for GLuint
	u32 mVBOTexture = 0;
	u32 mVBOColor = 0;
	u32 mVBOMatrices = 0;
	u32 mEBO = 0;

	u32 mMaxMeshesThreshold = 0;
	const u32 mMaxMeshesIncrement = 100;
	u32 mMeshesIndex = 0;

	bool mBinded = false;
	bool mDataSentToGPU = false;
	bool mIsWorldSpace = false;
	bool mIsStatic = false;

	bool mNewRendererAdded = false;
	bool mPendingDrawCall = false;
	bool mForceRegenerateBuffers = false;

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