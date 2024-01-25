#pragma once

#include "Core/Module.hpp"
#include "Graphics/GPU/GPUBuffersLayout.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/GPUMesh.hpp"

class GPUMeshBufferData
{
public:
	bool mIsStatic = false;
	bool mIsInstanced = false;
	bool mIsSkinned = false;
    Ptr<const GPUMesh> mMesh;
};

class GPUMeshBuffer
{
public:
	GPUMeshBuffer() = default;

    void init(const GPUMeshBufferData& gpuMeshBufferData);
    void terminate();
    void resizeMeshData(u32 maxInstances);
    void resizeInstancesData(u32 maxInstances);
    void setMeshData(Ptr<const GPUMesh> mesh);
    void setInstancesData(const std::vector<Matrix4>& matrices, const std::vector<u32>& instanceIDs);
    void setBonesTransforms(const std::vector<Matrix4>& transforms);
    void setIndicesData(Ptr<const GPUMesh> mesh);
    void enable();
    void disable();

private:
    GPUMeshBufferData mGPUMeshBufferData;
	GPUSharedBuffer mModelMatricesBuffer;
	GPUSharedBuffer mBonesMatricesBuffer;
    GPUBuffersLayout mGPUBuffersLayout;

    u32 mMaxInstances = 0;

public:
    CRGET(GPUBuffersLayout)
    CRGET(ModelMatricesBuffer)
    CRGET(BonesMatricesBuffer)
};
