#pragma once

#include "Core/Module.hpp"
#include "Graphics/GPU/GPUVertexBuffersLayout.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/GPUMesh.hpp"

class GPUMeshBufferData
{
public:
	bool mIsStatic = false;
	bool mIsInstanced = false;
	bool mUseVertexColor = false;
	bool mIsSkinned = false;
	GPUVertexInputBuffers mGPUVertexInputBuffers;
    u32 mVertexCount = 0;
};

class GPUMeshBuffer
{
public:
	GPUMeshBuffer() = default;
    ~GPUMeshBuffer();

    void init(const GPUMeshBufferData& gpuMeshBufferData);
    void resizeMeshData(Ptr<const GPUMesh> mesh);
    void resizeInstancesData(u32 maxInstances);
    void setMeshData(Ptr<const GPUMesh> mesh);
    void setInstancesData(const std::vector<Matrix4>& matrices, const std::vector<u32>& instanceIDs);
    void setBonesTransforms(const std::vector<Matrix4>& transforms);
    void setIndicesData(Ptr<const GPUMesh> mesh);
    void enable();
    void disable();

private:
    void terminate();

private:
    GPUMeshBufferData mGPUMeshBufferData;
	GPUSharedBuffer mModelMatricesBuffer;
	GPUSharedBuffer mBonesMatricesBuffer;
    GPUVertexBuffersLayout mGPUVertexBuffersLayout;

    u32 mMaxInstances = 0;

public:
    CRGET(GPUMeshBufferData)
    CRGET(GPUVertexBuffersLayout)
    CRGET(ModelMatricesBuffer)
    CRGET(BonesMatricesBuffer)
};
