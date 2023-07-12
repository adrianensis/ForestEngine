#pragma once

#include "Graphics/Buffers/GPUMeshBuffer.hpp"
#include "Graphics/Batch/BatchData.hpp"

class MeshBatcher: public ObjectBase
{
    GENERATE_METADATA(MeshBatcher)

public:
	MeshBatcher() = default;

    void init(const BatchData batchData);
    void resize(u32 size);
    void addInstance(const Matrix4& modelMatrix,Ptr<const Mesh> meshInstance);
    void drawCall();
    void enable();
    void disable();
    const GPUBuffersLayout& getGPUBuffersLayout() const { return mGPUMeshBuffer.getBuffersLayout(); }

private:
    void initInternal(u32 maxInstances);
    void initSingleMeshData();
    void resizeInternal(u32 maxInstances);
    void addMeshDataToBuffers(Ptr<const Mesh> meshInstance);
    void clear();
    void generateFacesData(u32 meshesCount);
    void sendDataToGPU();
	
public:
	GPUMeshBuffer mGPUMeshBuffer;

private:
    BatchData mBatchData;
	Mesh mMeshBuilder;
	std::vector<Matrix4> mMatrices;

	u32 mMaxMeshesThreshold = 0;
	const u32 mMaxMeshesIncrement = 100;
	u32 mMeshesIndex = 0;

	bool mDataSentToGPU = false;
};
