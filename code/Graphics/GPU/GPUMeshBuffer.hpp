#pragma once

#include "Core/Module.hpp"
#include "Graphics/GPU/GPUVertexBuffersLayout.hpp"
#include "Graphics/GPU/GPUUniformBlock.hpp"

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
    void setInstancesData(const std::vector<Matrix4>& matrices, const std::vector<u32>& instanceIDs);
    void setIndexesData(const Mesh& mesh);
    void enable();
    void disable();

private:
    void terminate();

private:
    GPUMeshBufferData mGPUMeshBufferData;
	GPUUniformBlock mModelMatricesBlock;

    u32 mVAO = 0;
    
    u32 mVBOPosition = 0;
	u32 mVBOTexture = 0;
	u32 mVBOColor = 0;
	u32 mVBOBonesIDs = 0;
	u32 mVBOBonesWeights = 0;
	u32 mVBOInstanceIDs = 0;

    GPUVertexBuffersLayout mGPUVertexBuffersLayout;

    u32 mMaxInstances = 0;

	u32 mEBO = 0;

public:
    CRGET(GPUVertexBuffersLayout)
    CRGET(ModelMatricesBlock)
};
