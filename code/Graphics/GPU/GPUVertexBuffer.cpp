#include "Graphics/GPU/GPUVertexBuffer.hpp"

void GPUVertexBuffer::init(u32 attributeLocation, const GPUVertexBufferData& data, bool isStatic)
{
	mData = data;
    mAttributeLocation = attributeLocation;
    mIsStatic = isStatic;

    // sizeInPrimitiveTypes: size of the object divided in primitive types
    // ex: Vector3 -> 3 floats
    u32 sizeInPrimitiveTypes = mData.mGPUVariableData.mGPUDataType.getSizePrimitiveType();
    // GET_SYSTEM(GPUInterface).attribute(getAttributeLocationWithOffset(), sizeInPrimitiveTypes, primitiveType, mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes, mPreviousOffsetInBytes, mData.mInstanceDivisor);
    // accumulative offset in bytes
    u32 primitiveTypeSizeInBytes = mData.mGPUVariableData.mGPUDataType.getPrimitiveTypeSizeInBytes();
    mPreviousOffsetInBytes = mPreviousOffsetInBytes + sizeInPrimitiveTypes * primitiveTypeSizeInBytes;
    mAttributeOffset += 1;
}

void GPUVertexBuffer::createBuffer()
{
    mBufferId = GET_SYSTEM(GPUInterface).createBuffer();
    
    u32 sizeInPrimitiveTypes = mData.mGPUVariableData.mGPUDataType.getSizePrimitiveType();
    GET_SYSTEM(GPUInterface).attribute(
        mAttributeLocation,
        sizeInPrimitiveTypes,
        mData.mGPUVariableData.mGPUDataType.mPrimitiveDataType,
        mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes,
        0/*mPreviousOffsetInBytes*/,
        mData.mInstanceDivisor
    );
}

void GPUVertexBuffer::terminate()
{
    GET_SYSTEM(GPUInterface).deleteBuffer(mBufferId);
}

void GPUVertexBuffer::resize(u32 size)
{
	GET_SYSTEM(GPUInterface).resizeBuffer(GPUBufferType::VERTEX, mBufferId, mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes, size, mIsStatic);
}

u32 GPUVertexBuffer::getAttributeLocation() const
{
    return mAttributeLocation;
}

u32 GPUVertexBuffer::getAttributeLocationWithOffset() const
{
    return getAttributeLocation() + mAttributeOffset;
}

void GPUIndicesBuffer::init(const GPUDataType& gpuDataType, bool isStatic)
{
    mIsStatic = isStatic;
    mGPUDataType = gpuDataType;
    mBufferId = GET_SYSTEM(GPUInterface).createBuffer();
}

void GPUIndicesBuffer::resize(u32 size)
{
    GET_SYSTEM(GPUInterface).resizeBuffer(GPUBufferType::INDEX, mBufferId, mGPUDataType.mTypeSizeInBytes, size, mIsStatic);
}

void GPUIndicesBuffer::terminate()
{
    GET_SYSTEM(GPUInterface).deleteBuffer(mBufferId);
}

