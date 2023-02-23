#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"

enum class GPUBufferPrimitiveType : u32
{
    INT = GL_INT,
    FLOAT = GL_FLOAT,
};

class GPUBuffer
{
public:
	GPUBuffer() = default;

    void init(u32 attributeIndex, u32 typeSizeInBytes, bool isStatic);
    void resize(u32 size);
    template <class T>
    void setData(const std::vector<T>& data)
    {
	    GET_SYSTEM(RenderContext).setDataVBOAnyType<T>(mVBO, data);
    }
    void setInstanceDivisor(u32 divisor) { mDivisor = divisor; };
    u32 attribute(GPUBufferPrimitiveType primitiveType, u32 customSizeInPrimitiveTypes = 0);
    u32 getAttributeLocation() const;
    u32 getAttributeLocationWithOffset() const;
    void terminate();

private:
	bool mIsStatic = false;
	u32 mVBO = 0; // TODO: change u32 for GLuint
    u32 mAttribute = 0;
    u32 mAttributeOffset = 0;
    u32 mPreviousOffsetInBytes = 0;
    u32 mTypeSizeInBytes = 0;
    u32 mDivisor = 0;
};