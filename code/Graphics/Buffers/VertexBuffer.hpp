#ifndef VERTEXBUFFER_HPP
#define VERTEXBUFFER_HPP

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"

class VertexBuffer: public ObjectBase
{
    GENERATE_METADATA(VertexBuffer)

public:
	VertexBuffer() = default;
    ~VertexBuffer() override;

    void init(u32 elementSize, u32 propertyArrayIndex, bool isStatic);
    void terminate();
    void resize(u32 size);
    void setData(const std::vector<f32> &data);

private:
	bool mGenerated = false;

	bool mIsStatic = false;
	u32 mVBO = 0; // TODO: change u32 for GLuint
	u32 mAttribPointerIndex = 0;
};

class BonesBuffer: public ObjectBase
{
    GENERATE_METADATA(BonesBuffer)

public:
	BonesBuffer() = default;
    ~BonesBuffer() override;

    void init(u32 propertyArrayIndex, bool isStatic);
    void terminate();
    void resize(u32 size);
    void setData(const std::vector<BoneVertexData> &data);

private:
	bool mGenerated = false;

	bool mIsStatic = false;
	u32 mVBO = 0; // TODO: change u32 for GLuint
	u32 mAttribPointerIndex = 0;
};

// TODO: Move Instanced Rendering matrix to MatrixBuffer

// class MatrixBuffer: public ObjectBase
// {
//     GENERATE_METADATA(MatrixBuffer)

// public:
// 	MatrixBuffer() = default;
//     ~MatrixBuffer() override;

//     void init(u32 elementSize, u32 propertyArrayIndex, bool isStatic);
//     void terminate();
//     void resize(u32 size);
//     void setData(const std::vector<Matrix4> &data);

// private:
// 	bool mGenerated = false;

// 	bool mIsStatic = false;
// 	u32 mVBO = 0; // TODO: change u32 for GLuint
// 	u32 mAttribPointerIndex = 0;
// };

class MeshBuffer: public ObjectBase
{
    GENERATE_METADATA(MeshBuffer)

public:
	MeshBuffer() = default;
	
    ~MeshBuffer() override;

    void init(bool isStatic, bool isInstanced);
    void terminate();
    void resize(const Mesh& mesh);
    void setData(const Mesh& mesh);
    void setIndexesData(const Mesh& mesh);
    void addInstanceMatrix(const Matrix4& modelMatrix);
    void clear();
    void setMaxInstances(u32 maxInstances);
    void enable();
    void disable();

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
    BonesBuffer mVBOBones;
	u32 mEBO = 0;
public:
	GET(IsInstanced)
};

class MeshBatcher: public ObjectBase
{
    GENERATE_METADATA(MeshBatcher)

public:
	MeshBatcher() = default;
    ~MeshBatcher() override;

    void init(Ptr<const Mesh> prototypeMesh, bool isStatic, bool isInstanced);
    void resize(u32 size);
    void addInstanceMatrix(const Matrix4& modelMatrix);
    void addInstance(const Mesh& meshInstance);
    void drawCall();
    void terminate();
    void enable();
    void disable();

private:
    void clear();
    void generateFacesData(u32 meshesCount);
    void sendDataToGPU();
	
private:
	Ptr<const Mesh> mPrototypeMesh;
	Mesh mMeshBuilder;
	MeshBuffer mMeshBuffer;

	u32 mMaxMeshesThreshold = 0;
	const u32 mMaxMeshesIncrement = 100;

	u32 mMeshesIndex = 0;

	bool mDataSentToGPU = false;

public:
	RGET(MeshBuilder)
};

#endif