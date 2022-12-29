#ifndef VERTEXBUFFER_HPP
#define VERTEXBUFFER_HPP

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"

enum class GPUBufferPrimitiveType
{
    INT = GL_INT,
    FLOAT = GL_FLOAT,
};

class GPUBufferBase
{
public:
	GPUBufferBase() = default;
    ~GPUBufferBase();

    void resize(u32 size);
    void attribute(u32 attributeIndex, GPUBufferPrimitiveType primitiveType, u32 pointerOffset, u32 divisor);
    void attributeCustomSize(u32 attributeIndex, GPUBufferPrimitiveType primitiveType, u32 elementSize, u32 pointerOffset, u32 divisor);

protected:
    void init(u32 typeSizeInBytes, bool isStatic);
    void terminate();

protected:
	bool mGenerated = false;

	bool mIsStatic = false;
	u32 mVBO = 0; // TODO: change u32 for GLuint

    u32 mTypeSizeInBytes = 0;
};

template <class T>
class GPUBuffer: public GPUBufferBase
{
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

#endif