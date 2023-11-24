#include "Graphics/GPU/GPUUniformBlock.hpp"

GPUVariableData GPUUniformBlockData::getScopedGPUVariableData(u32 i) const
{
    GPUVariableData data = mGPUVariableDefinitionDataArray[i];
    data.mName = mInstanceName + "." + data.mName;
    return data;
}

void GPUUniformBlock::init(u32 bindingPoint, const GPUUniformBlockData& gpuBlockData, bool isStatic)
{
	mGPUUniformBlockData = gpuBlockData;
    mBindingPoint = bindingPoint;
    mIsStatic = isStatic;
	mUBO = GET_SYSTEM(GPUInterface).createUBO();

    mSizeInBytes = 0;
    FOR_ARRAY(i, mGPUUniformBlockData.mGPUVariableDefinitionDataArray)
    {
        const GPUVariableData& gpuVariableData = mGPUUniformBlockData.mGPUVariableDefinitionDataArray[i];
        mSizeInBytes += gpuVariableData.mGPUDataType.mTypeSizeInBytes;
    }

	GET_SYSTEM(GPUInterface).bindUBO(mUBO, mBindingPoint);
}

void GPUUniformBlock::terminate()
{
    GET_SYSTEM(GPUInterface).deleteUBO(mUBO);
}

void GPUUniformBlock::resize(u32 size)
{
	GET_SYSTEM(GPUInterface).resizeUBOAnyType(mUBO, mSizeInBytes, size, mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}