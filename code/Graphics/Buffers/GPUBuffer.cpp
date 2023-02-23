#include "Graphics/Buffers/GPUBuffer.hpp"

GPUBuffer::~GPUBuffer() 
{
	terminate();
}

void GPUBuffer::init(u32 attributeIndex, u32 typeSizeInBytes, bool isStatic)
{
	mTypeSizeInBytes = typeSizeInBytes;
	mIsStatic = isStatic;
    mAttribute = attributeIndex;

	mVBO = GET_SYSTEM(RenderContext).createVBO();
}

void GPUBuffer::terminate()
{
    GET_SYSTEM(RenderContext).deleteVBO(mVBO);
}

void GPUBuffer::resize(u32 size)
{
	GET_SYSTEM(RenderContext).resizeVBOAnyType(mVBO, mTypeSizeInBytes, size, mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}

u32 GPUBuffer::attribute(GPUBufferPrimitiveType primitiveType, u32 pointerOffset, u32 divisor)
{
	u32 primitiveTypeSize = 0;
	switch (primitiveType)
	{
		case GPUBufferPrimitiveType::FLOAT:
			primitiveTypeSize = sizeof(f32);
		break;
		case GPUBufferPrimitiveType::INT:
			primitiveTypeSize = sizeof(u32);
		break;
	}

	return attributeCustomSize(primitiveType, mTypeSizeInBytes/primitiveTypeSize, pointerOffset, divisor);
}

u32 GPUBuffer::attributeCustomSize(GPUBufferPrimitiveType primitiveType, u32 elementSize, u32 pointerOffset, u32 divisor)
{
	GET_SYSTEM(RenderContext).attribute(getAttributeLocationWithOffset(), elementSize, (u32)primitiveType, mTypeSizeInBytes, pointerOffset, divisor);
    mAttributeOffset += 1;
    return getAttributeLocationWithOffset();
}

u32 GPUBuffer::getAttributeLocation() const
{
    return mAttribute;
}

u32 GPUBuffer::getAttributeLocationWithOffset() const
{
    return getAttributeLocation() + mAttributeOffset;
}