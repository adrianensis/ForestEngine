#pragma once

#include "Core/Module.hpp"
#include "Graphics/GPU/GPUBuffersLayout.hpp"

class Mesh;

class GPUMeshBufferData
{
public:
	bool mIsStatic = false;
	bool mIsInstanced = false;
	bool mUseVertexColor = false;
    u32 mVertexCount = 0;
};

class GPUMeshBuffer
{
public:
	GPUMeshBuffer() = default;
	
    ~GPUMeshBuffer();

    void init(const GPUMeshBufferData& gpuMeshBufferData);
    void resizeMeshData(const Mesh& mesh);
    void resizeInstancesData(u32 maxInstances);
    void setMeshData(const Mesh& mesh);
    void setInstancesData(const std::vector<Matrix4>& matrices);
    void setIndexesData(const Mesh& mesh);
    void enable();
    void disable();

private:
    void terminate();

private:
    GPUMeshBufferData mGPUMeshBufferData;
    u32 mVAO = 0;
    
    u32 mVBOPosition = 0;
	u32 mVBOTexture = 0;
	u32 mVBOColor = 0;
	u32 mVBOBonesIDs = 0;
	u32 mVBOBonesWeights = 0;
	u32 mVBOModelMatrix = 0;

    GPUBuffersLayout mBuffersLayout;

    u32 mMaxInstances = 0;

	u32 mEBO = 0;

public:
    CRGET(BuffersLayout)
};