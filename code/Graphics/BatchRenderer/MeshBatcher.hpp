#pragma once

#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/BatchRenderer/BatchData.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"

class MeshBatcher
{
public:
	MeshBatcher() = default;

    void init(const BatchData batchData);
    void terminate();
    void resize(u32 size);
    void addInstanceData(Ptr<MeshRenderer> renderer);
    void drawCall();
    void enable();
    void disable();

private:
    void initInternal(u32 maxInstances);
    void resizeInternal(u32 maxInstances);
    void appendMeshData(Ptr<const Mesh> mesh);
    void clear();
    void generateIndicesData(u32 meshesCount);
    void generateInstanceIDsData(u32 meshesCount);
    void updateBoneTransforms();
    void sendDataToGPU();

    void initBuffers();
    void resizeMeshBuffers(u32 maxInstances);
    void resizeInstancedBuffers(u32 maxInstances);
    void setMeshBuffers(Ptr<const GPUMesh> mesh);
    void setInstancedBuffers();
    void setBonesTransformsBuffer(const std::vector<Matrix4>& transforms);
    void setIndicesBuffer(Ptr<const GPUMesh> mesh);

private:
    BatchData mBatchData;
	OwnerPtr<Mesh> mInternalMesh;
    std::vector<u32> mInstanceIDs;
    std::vector<u32> mObjectIDs;
    std::vector<u32> mMaterialInstanceIDs;

    GPUVertexBuffersContainer mGPUVertexBuffersContainer;
    GPUSharedBuffersContainer mGPUSharedBuffersContainer;

	u32 mMaxMeshesThreshold = 0;
	const u32 mMaxMeshesIncrement = 100;
	u32 mMeshesIndex = 0;

	bool mDataSentToGPU = false;

public:
    RGET(GPUVertexBuffersContainer)
    CRGET(GPUVertexBuffersContainer)
    RGET(GPUSharedBuffersContainer)
    CRGET(GPUSharedBuffersContainer)
};
