#include "Graphics/Buffers/GPUBuffer.hpp"

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

u32 GPUBuffer::attribute(GPUBufferPrimitiveType primitiveType, u32 customSizeInPrimitiveTypes /*=0*/)
{
	u32 primitiveTypeSize = 0;
	switch (primitiveType)
	{
		case GPUBufferPrimitiveType::FLOAT:
			primitiveTypeSize = sizeof(f32);
		break;
		case GPUBufferPrimitiveType::INT:
			primitiveTypeSize = sizeof(i32);
		break;
	}

    // sizeInPrimitiveTypes: size of the object divided in primitive types
    // ex: Vector3 -> 3 floats
    u32 sizeInPrimitiveTypes = mTypeSizeInBytes/primitiveTypeSize;
    if(customSizeInPrimitiveTypes > 0)
    {
        sizeInPrimitiveTypes = customSizeInPrimitiveTypes;
    }

    GET_SYSTEM(RenderContext).attribute(getAttributeLocationWithOffset(), sizeInPrimitiveTypes, static_cast<u32>(primitiveType), mTypeSizeInBytes, mPreviousOffsetInBytes, mDivisor);
    // accumulative offset in bytes
    mPreviousOffsetInBytes = mPreviousOffsetInBytes + sizeInPrimitiveTypes * primitiveTypeSize;
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