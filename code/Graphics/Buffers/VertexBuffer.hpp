#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"

// A single buffer of data
class VertexBuffer: public ObjectBase
{
    GENERATE_METADATA(VertexBuffer)

private:
	bool mGenerated = false;

	bool mIsStatic = false;
	u32 mVBO = 0; // TODO: change u32 for GLuint
	u32 mAttribPointerIndex = 0;

public:
	VertexBuffer() = default;
	~VertexBuffer() override;
	void init(u32 elementSize, u32 propertyArrayIndex, bool isStatic);
	void terminate();
	void resize(u32 size);
	void setData(const std::vector<f32> &data);
};

// Buffers data for a mesh
class MeshBuffer: public ObjectBase
{
    GENERATE_METADATA(MeshBuffer)

private:
	bool mGenerated = false;

	bool mIsStatic = false;

	u32 mVBOMatrices = 0;
	bool mIsInstanced = false;
	std::vector<f32> mMatrices;
	
	u32 mVAO = 0;
	VertexBuffer mVBOPosition;
	VertexBuffer mVBOTexture;
	VertexBuffer mVBOColor;
	u32 mEBO = 0;

public:
	MeshBuffer() = default;
	~MeshBuffer() override;
	void init(bool isStatic, bool isInstanced);
	void terminate();
	void resize(const Mesh& mesh);
	void setData(const Mesh& mesh);
	void setIndexesData(const Mesh& mesh);
	void addInstanceMatrix(const Matrix4& modelMatrix);
	void setMaxInstances(u32 maxInstances);
	void clear();
	void enable();
	void disable();

	GET(IsInstanced)
};

// Combines all the data of multiple instances of the same Mesh
class MeshBatcher: public ObjectBase
{
    GENERATE_METADATA(MeshBatcher)

private:
	Ref<const Mesh> mPrototypeMesh;
	Mesh mMeshBuilder;
	MeshBuffer mMeshBuffer;

	u32 mMaxMeshesThreshold = 0;
	const u32 mMaxMeshesIncrement = 100;

	u32 mMeshesIndex = 0;

	bool mDataSentToGPU = false;

	void clear();
	void generateFacesData(u32 meshesCount);
	void sendDataToGPU();

public:
	MeshBatcher() = default;
	~MeshBatcher() override;
	void init(Ref<const Mesh> prototypeMesh, bool isStatic, bool isInstanced);
	void terminate();
	void resize(u32 size);
	void addInstanceMatrix(const Matrix4& modelMatrix);
	void addInstance(const Mesh& meshInstance);

	void drawCall();

	void enable();
	void disable();

	RGET(MeshBuilder)
};