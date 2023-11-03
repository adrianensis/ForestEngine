#include "Graphics/GPU/GPUUniformBlock.hpp"

GPUVariableData GPUUniformBlockData::getScopedGPUVariableData(u32 i) const
{
    GPUVariableData data = mGPUVariableDataArray[i];
    data.mName = mInstanceName + "." + data.mName;
    return data;
}

void GPUUniformBlock::init(u32 bindingPoint, const GPUUniformBlockData& gpuBlockData, bool isStatic)
{
	mGPUUniformBlockData = gpuBlockData;
    mIsStatic = isStatic;
	mUBO = GET_SYSTEM(GPUInterface).createUBO();
    mBindingPoint = bindingPoint;

    mSizeInBytes = 0;
    FOR_ARRAY(i, mGPUUniformBlockData.mGPUVariableDataArray)
    {
        const GPUVariableData& gpuVariableData = mGPUUniformBlockData.mGPUVariableDataArray[i];
        mSizeInBytes += gpuVariableData.mGPUDataType.mTypeSizeInBytes;
    }

	GET_SYSTEM(GPUInterface).bindUBO(mUBO, mBindingPoint);
    resize(mSizeInBytes);
}

void GPUUniformBlock::terminate()
{
    GET_SYSTEM(GPUInterface).deleteUBO(mUBO);
}

void GPUUniformBlock::resize(u32 size)
{
	GET_SYSTEM(GPUInterface).resizeUBOAnyType(mUBO, mSizeInBytes, 1, mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}