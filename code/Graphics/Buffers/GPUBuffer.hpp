#pragma once

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
