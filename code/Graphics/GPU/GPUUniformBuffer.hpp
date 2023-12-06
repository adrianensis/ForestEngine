#pragma once

#include "Graphics/GPU/GPUVariable.hpp"
#include "Graphics/GPU/GPUInterface.hpp"

class GPUUniformBufferData
{
public:
	GPUUniformBufferData() = default;
	GPUUniformBufferData(const std::vector<GPUVariableDefinitionData>& gpuVariableDefinitionDataArray, const std::string& blockName):
        mGPUVariableDefinitionDataArray(gpuVariableDefinitionDataArray), mBlockName(blockName) {}
	GPUUniformBufferData(const std::vector<GPUVariableDefinitionData>& gpuVariableDefinitionDataArray, const std::string& blockName, const std::string& instanceName):
        mGPUVariableDefinitionDataArray(gpuVariableDefinitionDataArray), mBlockName(blockName), mInstanceName(instanceName) {}

public:
    std::vector<GPUVariableDefinitionData> mGPUVariableDefinitionDataArray;
    std::string mBlockName;
    std::string mInstanceName;

public:
    GPUVariableData getScopedGPUVariableData(u32 i) const;
};

class GPUUniformBuffer
{
public:
	GPUUniformBuffer() = default;

    void init(u32 bindingPoint, const GPUUniformBufferData& gpuBlockData, bool isStatic);
    void resize(u32 size);
    template <class T>
    void setData(const T& data)
    {
	    GET_SYSTEM(GPUInterface).setDataUBOAnyStruct(mUBO, data);
    }
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
	    GET_SYSTEM(GPUInterface).setDataUBOAnyType(mUBO, data);
    }
    void terminate();

protected:
	u32 mBindingPoint = 0;
    GPUUniformBufferData mGPUUniformBufferData;
	u32 mUBO = 0;
    bool mIsStatic = false;
    u32 mSizeInBytes = 0;
public:
    GET(SizeInBytes)
    GET(BindingPoint)
    CRGET(GPUUniformBufferData)
};