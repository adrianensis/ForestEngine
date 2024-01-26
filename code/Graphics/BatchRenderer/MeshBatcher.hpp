#pragma once

#include "Graphics/GPU/GPUBuffersLayout.hpp"
#include "Graphics/BatchRenderer/BatchData.hpp"

class MeshBatcher: public ObjectBase
{
    GENERATE_METADATA(MeshBatcher)

public:
	MeshBatcher() = default;

    void init(const BatchData batchData);
    void terminate();
    void resize(u32 size);
    void addInstance(const Matrix4& modelMatrix,Ptr<const Mesh> meshInstance);
    void updateBoneTransforms();
    void drawCall();
    void enable();
    void disable();

private:
    void initInternal(u32 maxInstances);
    void resizeInternal(u32 maxInstances);
    void addInstanceMeshData(Ptr<const Mesh> meshInstance);
    void clear();
    void generateIndicesData(u32 meshesCount);
    void generateInstanceIDsData(u32 meshesCount);
    void sendDataToGPU();

    void initBuffers();
    void resizeBuffers(u32 maxInstances);
    void resizeInstancedBuffers(u32 maxInstances);
    void setMeshBuffers(Ptr<const GPUMesh> mesh);
    void setInstancedBuffers(const std::vector<Matrix4>& matrices, const std::vector<u32>& instanceIDs);
    void setBonesTransformsBuffer(const std::vector<Matrix4>& transforms);
    void setIndicesBuffer(Ptr<const GPUMesh> mesh);
	
public:
	// GPUMeshBuffer mGPUMeshBuffer;
    GPUBuffersLayout mGPUBuffersLayout;

private:
    BatchData mBatchData;
	OwnerPtr<Mesh> mInternalMesh;
	std::vector<Matrix4> mMatrices;
    std::vector<u32> mInstanceIDs;

	GPUSharedBuffer mBonesMatricesBuffer;

	u32 mMaxMeshesThreshold = 0;
	const u32 mMaxMeshesIncrement = 100;
	u32 mMeshesIndex = 0;

	bool mDataSentToGPU = false;

public:
    CRGET(GPUBuffersLayout)
    CRGET(BonesMatricesBuffer)
};
