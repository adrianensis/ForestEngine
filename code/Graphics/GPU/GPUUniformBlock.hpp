#pragma once

#include "Graphics/GPU/GPUVariable.hpp"
#include "Graphics/GPU/GPUInterface.hpp"

class GPUUniformBlockData
{
public:
	GPUUniformBlockData() = default;
	GPUUniformBlockData(const std::vector<GPUVariableData>& gpuVariableDataArray, const std::string& blockName): mGPUVariableDataArray(gpuVariableDataArray), mBlockName(blockName) {}
	GPUUniformBlockData(const std::vector<GPUVariableData>& gpuVariableDataArray, const std::string& blockName, const std::string& instanceName): mGPUVariableDataArray(gpuVariableDataArray), mBlockName(blockName), mInstanceName(instanceName) {}

public:
    std::vector<GPUVariableData> mGPUVariableDataArray;
    std::string mBlockName;
    std::string mInstanceName;

public:
    GPUVariableData getScopedGPUVariableData(u32 i) const;
};

class GPUUniformBlock
{
public:
	GPUUniformBlock() = default;

    void init(u32 bindingPoint, const GPUUniformBlockData& gpuBlockData, bool isStatic);
    void resize(u32 size);
    template <class T>
    void setData(const T& data)
    {
	    GET_SYSTEM(GPUInterface).setDataUBOAnyStruct(mUBO, data);
    }
    void terminate();

protected:
    GPUUniformBlockData mGPUUniformBlockData;
	u32 mBindingPoint = 0;
	u32 mUBO = 0;
    bool mIsStatic = false;
    u32 mSizeInBytes = 0;
public:
    GET(SizeInBytes)
    CRGET(GPUUniformBlockData)
};