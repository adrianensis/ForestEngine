#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/Mesh/MeshBatcher.hpp"
#include "Graphics/BatchRenderer/BatchData.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"

class BatchRenderer: public ObjectBase
{
public:
    void init(const BatchData& batchData);
    void terminate();

    void render();
    void onAddRenderer(Ptr<MeshRenderer> renderer);
    void onRemoveRenderer(Ptr<MeshRenderer> renderer);

private:
    void bindSharedBuffers();
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
	std::vector<Ptr<MeshRenderer>> mRenderers;
    OwnerPtr<GPUProgram> mShader;
	MeshBatcher mMeshBatcher;
    BatchData mBatchData;

    GPUVertexBuffersContainer mGPUVertexBuffersContainer;
    GPUSharedBuffersContainer mGPUSharedBuffersContainer;

	u32 mMaxMeshesThreshold = 0;
	const u32 mMaxMeshesIncrement = 100;
	bool mRegenerateBuffersRequested = false;
	bool mDataSubmittedToGPU = false;

public:
    CRGET(BatchData)
};
REGISTER_CLASS(BatchRenderer);
