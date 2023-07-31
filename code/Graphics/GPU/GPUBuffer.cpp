#include "Graphics/GPU/GPUBuffer.hpp"

void GPUBuffer::init(u32 attributeLocation, const GPUBufferData& data, bool isStatic)
{
	mData = data;
    mAttributeLocation = attributeLocation;
    mIsStatic = isStatic;
	mVBO = GET_SYSTEM(RenderContext).createVBO();

    u32 sizeInPrimitiveTypes = mData.mGPUAttributeData.mGPUDataType.getSizePrimitiveType();
    if(mData.mAttributeDivisorSizeInPrimitiveTypes > 0)
    {
        u32 n = sizeInPrimitiveTypes/mData.mAttributeDivisorSizeInPrimitiveTypes;
        FOR_RANGE(i, 0, n)
        {
            attribute(mData.mGPUAttributeData.mGPUDataType.mPrimitiveType, mData.mAttributeDivisorSizeInPrimitiveTypes);
        }
    }
    else
    {
        attribute(mData.mGPUAttributeData.mGPUDataType.mPrimitiveType, mData.mAttributeDivisorSizeInPrimitiveTypes);
    }
}

void GPUBuffer::terminate()
{
    GET_SYSTEM(RenderContext).deleteVBO(mVBO);
}

void GPUBuffer::resize(u32 size)
{
	GET_SYSTEM(RenderContext).resizeVBOAnyType(mVBO, mData.mGPUAttributeData.mGPUDataType.mTypeSizeInBytes, size, mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}

u32 GPUBuffer::attribute(GPUPrimitiveType primitiveType, u32 customSizeInPrimitiveTypes /*=0*/)
{
    // sizeInPrimitiveTypes: size of the object divided in primitive types
    // ex: Vector3 -> 3 floats
    u32 sizeInPrimitiveTypes = mData.mGPUAttributeData.mGPUDataType.getSizePrimitiveType();
    if(customSizeInPrimitiveTypes > 0)
    {
        sizeInPrimitiveTypes = customSizeInPrimitiveTypes;
    }

    GET_SYSTEM(RenderContext).attribute(getAttributeLocationWithOffset(), sizeInPrimitiveTypes, static_cast<u32>(primitiveType), mData.mGPUAttributeData.mGPUDataType.mTypeSizeInBytes, mPreviousOffsetInBytes, mData.mInstanceDivisor);
    // accumulative offset in bytes
    u32 primitiveTypeSizeInBytes = mData.mGPUAttributeData.mGPUDataType.getPrimitiveTypeSizeInBytes();
    mPreviousOffsetInBytes = mPreviousOffsetInBytes + sizeInPrimitiveTypes * primitiveTypeSizeInBytes;
    mAttributeOffset += 1;
    return getAttributeLocationWithOffset();
}

u32 GPUBuffer::getAttributeLocation() const
{
    return mAttributeLocation;
}

u32 GPUBuffer::getAttributeLocationWithOffset() const
{
    return getAttributeLocation() + mAttributeOffset;
}