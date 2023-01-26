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
	bool mGenerated = false;

	bool mIsStatic = false;
	bool mIsInstanced = false;

	u32 mVAO = 0;

	GPUBuffer<Vector3> mVBOPosition;
	GPUBuffer<Vector2> mVBOTexture;
	GPUBuffer<Vector4> mVBOColor;
	GPUBuffer<BoneVertexData> mVBOBone;
    
	std::vector<Matrix4> mMatrices;
	GPUBuffer<Matrix4> mVBOModelMatrix;

	u32 mEBO = 0;
};
