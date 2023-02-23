#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"

enum class GPUBufferPrimitiveType
{
    INT = GL_INT,
    FLOAT = GL_FLOAT,
};

class GPUBuffer
{
public:
	GPUBuffer() = default;
    ~GPUBuffer();

    void init(u32 attributeIndex, u32 typeSizeInBytes, bool isStatic);
    void resize(u32 size);
    template <class T>
    void setData(const std::vector<T>& data)
    {
	    GET_SYSTEM(RenderContext).setDataVBOAnyType<T>(mVBO, data);
    }
    u32 attribute(GPUBufferPrimitiveType primitiveType, u32 pointerOffset, u32 divisor);
    u32 attributeCustomSize(GPUBufferPrimitiveType primitiveType, u32 elementSize, u32 pointerOffset, u32 divisor);
    u32 getAttributeLocation() const;
    u32 getAttributeLocationWithOffset() const;

protected:
    void terminate();

protected:
	bool mIsStatic = false;
	u32 mVBO = 0; // TODO: change u32 for GLuint
    u32 mAttribute = 0;
    u32 mAttributeOffset = 0;
    u32 mTypeSizeInBytes = 0;
};