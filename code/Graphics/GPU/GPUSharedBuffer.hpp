#pragma once

#include "Graphics/GPU/GPUVariable.hpp"
#include "Graphics/GPU/GPUInterface.hpp"

class GPUSharedBufferData
{
public:
	GPUSharedBufferData() = default;
	GPUSharedBufferData(GPUBufferType gpuSharedBufferType, const std::vector<GPUVariableDefinitionData>& gpuVariableDefinitionDataArray, const std::string& blockName):
        mType(gpuSharedBufferType), mGPUVariableDefinitionDataArray(gpuVariableDefinitionDataArray), mBufferName(blockName) {}
	GPUSharedBufferData(GPUBufferType gpuSharedBufferType, const std::vector<GPUVariableDefinitionData>& gpuVariableDefinitionDataArray, const std::string& blockName, const std::string& instanceName):
        mType(gpuSharedBufferType), mGPUVariableDefinitionDataArray(gpuVariableDefinitionDataArray), mBufferName(blockName), mInstanceName(instanceName) {}

    GPUVariableData getScopedGPUVariableData(u32 i) const;

public:
    GPUBufferType mType = GPUBufferType::UNIFORM;
    std::vector<GPUVariableDefinitionData> mGPUVariableDefinitionDataArray;
    std::string mBufferName;
    std::string mInstanceName;
};

class GPUSharedBuffer
{
public:
	GPUSharedBuffer() = default;

    void init(u32 bindingPoint, const GPUSharedBufferData& gpuBufferData, bool isStatic);
    void resize(u32 size);
    template <class T>
    void setDataStruct(const T& data)
    {
        GET_SYSTEM(GPUInterface).setBufferDataStruct(mGPUSharedBufferData.mType, mBufferId, data);
    }
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
        GET_SYSTEM(GPUInterface).setBufferDataArray(mGPUSharedBufferData.mType, mBufferId, data);
    }
    void terminate();

private:
	u32 mBindingPoint = 0;
    GPUSharedBufferData mGPUSharedBufferData;
	u32 mBufferId = 0;
    bool mIsStatic = false;
    u32 mSizeInBytes = 0;
public:
    GET(SizeInBytes)
    GET(BindingPoint)
    CRGET(GPUSharedBufferData)
};