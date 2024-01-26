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
    void initSingleMeshData();
    void resizeInternal(u32 maxInstances);
    void addMeshDataToBuffers(Ptr<const Mesh> meshInstance);
    void clear();
    void generateIndicesData(u32 meshesCount);
    void generateInstanceIDsData(u32 meshesCount);
    void sendDataToGPU();

    void initBuffers();
    void resizeMeshData(u32 maxInstances);
    void resizeInstancesData(u32 maxInstances);
    void setMeshData(Ptr<const GPUMesh> mesh);
    void setInstancesData(const std::vector<Matrix4>& matrices, const std::vector<u32>& instanceIDs);
    void setBonesTransforms(const std::vector<Matrix4>& transforms);
    void setIndicesData(Ptr<const GPUMesh> mesh);
	
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
