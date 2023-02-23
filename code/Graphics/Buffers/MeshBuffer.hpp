#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Buffers/GPUBuffer.hpp"

class MeshBuffer
{
public:
	MeshBuffer() = default;
	
    ~MeshBuffer();

    void init(bool isStatic, bool isInstanced);
    void resize(const Mesh& mesh);
    void setData(const Mesh& mesh);
    void setIndexesData(const Mesh& mesh);
    void addInstanceMatrix(const Matrix4& modelMatrix);
    void setDataInstanced();
    void clear();
    void setMaxInstances(u32 maxInstances);
    void enable();
    void disable();

private:
    void terminate();

public:
	bool mIsStatic = false;
	bool mIsInstanced = false;

	u32 mVAO = 0;

	GPUBuffer mVBOPosition;
	GPUBuffer mVBOTexture;
	GPUBuffer mVBOColor;
	GPUBuffer mVBOBone;
    
	std::vector<Matrix4> mMatrices;
	GPUBuffer mVBOModelMatrix;

	u32 mEBO = 0;
};
