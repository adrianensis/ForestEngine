#ifndef VERTEXBUFFER_HPP
#define VERTEXBUFFER_HPP

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"

enum class GPUBufferPrimitiveType
{
    INT = GL_INT,
    FLOAT = GL_FLOAT,
};

class GPUBufferBase: public ObjectBase
{
    GENERATE_METADATA(GPUBufferBase)

public:
	GPUBufferBase() = default;
    ~GPUBufferBase() override;

    void terminate();
    void resize(u32 size);
    void attribute(u32 attributeIndex, GPUBufferPrimitiveType primitiveType, u32 pointerOffset, u32 divisor);
    void attributeCustomSize(u32 attributeIndex, GPUBufferPrimitiveType primitiveType, u32 elementSize, u32 pointerOffset, u32 divisor);

protected:
    void init(u32 typeSizeInBytes, bool isStatic);

protected:
	bool mGenerated = false;

	bool mIsStatic = false;
	u32 mVBO = 0; // TODO: change u32 for GLuint

    u32 mTypeSizeInBytes = 0;
};

template <class T>
class GPUBuffer: public GPUBufferBase
{
    GENERATE_METADATA(GPUBuffer<T>)

public:
    void init(bool isStatic)
    {
        GPUBufferBase::init(sizeof(T), isStatic);
    }

    void setData(const std::vector<T>& data)
    {
	    RenderContext::setDataVBOAnyType<T>(mVBO, data);
    }

    void resize(u32 size)
    {
        GPUBufferBase::resize(size);
    }
};

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
    void setDataInstanced();
    void clear();
    void setMaxInstances(u32 maxInstances);
    void enable();
    void disable();

private:
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
    GET(PrototypeMesh)
};

#endif