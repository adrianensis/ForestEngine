#pragma once

#include "Core/Module.hpp"
#include "Graphics/Buffers/GPUBuffersLayout.hpp"

class Mesh;

class GPUMeshBuffer
{
public:
	GPUMeshBuffer() = default;
	
    ~GPUMeshBuffer();

    void init(u32 vertexCount, bool isStatic, bool isInstanced, bool useVertexColor);
    void resize(const Mesh& mesh);
    void setData(const Mesh& mesh);
    void setIndexesData(const Mesh& mesh);
    void addInstanceMatrix(const Matrix4& modelMatrix);
    void clear();
    void setMaxInstances(u32 maxInstances);
    void enable();
    void disable();

private:
    void terminate();

public:
	bool mIsStatic = false;
	bool mIsInstanced = false;
	bool mUseVertexColor = false;
    u32 mVertexCount = 0;
	
    u32 mVAO = 0;
    
	std::vector<Matrix4> mMatrices;

    u32 mVBOPosition = 0;
	u32 mVBOTexture = 0;
	u32 mVBOColor = 0;
	u32 mVBOBonesIDs = 0;
	u32 mVBOBonesWeights = 0;
	u32 mVBOModelMatrix = 0;

    GPUBuffersLayout mBuffersLayout;

	u32 mEBO = 0;
};
