#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/Mesh/MeshBatcher.hpp"
#include "Graphics/Renderer/BatchRenderer/BatchData.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/Material/Shader/Shader.hpp"

class GPUProgram;

class BatchRenderer: public ObjectBase
{
public:
    void init(const BatchData& batchData);
    void terminate();

    void render();
    void addRenderer(Ptr<MeshRenderer> renderer);
    void removeRenderer(Ptr<MeshRenderer> renderer);

    bool isEmpty() const { return mRenderersCount == 0; }

private:
    void enable();
    void disable();
    void updateBuffers();
    bool shouldRegenerateBuffers() const;
    void updateBoneTransforms();

    void initBuffers();
    void resizeMeshBuffers(u32 maxInstances);
    void resizeInstancedBuffers(u32 maxInstances);
    void setMeshBuffers(Ptr<const GPUMesh> mesh);
    void setInstancedBuffers();
    void setBonesTransformsBuffer(const std::vector<Matrix4>& transforms);
    void resizeIndicesBuffer(Ptr<const GPUMesh> mesh);
    void setIndicesBuffer(Ptr<const GPUMesh> mesh);
    void drawCall();

private:
    SlotsManager mRendererSlotsManager;
	std::vector<Ptr<MeshRenderer>> mRenderers;
    u32 mRenderersCount = 0;
    inline static const u32 mInitialInstances = 2000;

	MeshBatcher mMeshBatcher;
    BatchData mBatchData;

    GPUVertexBuffersContainer mGPUVertexBuffersContainer;

	u32 mMaxMeshesThreshold = 0;
	static const u32 smMeshesIncrement = 100;
	bool mRegenerateBuffersRequested = false;
	bool mDataSubmittedToGPU = false;

public:
    CRGET(BatchData)
    CRGET(GPUVertexBuffersContainer)
};
REGISTER_CLASS(BatchRenderer);
