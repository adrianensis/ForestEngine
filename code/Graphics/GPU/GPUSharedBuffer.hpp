#pragma once

#include "Graphics/GPU/GPUVariable.hpp"
#include "Graphics/GPU/GPUInterface.hpp"

enum GPUSharedBufferType
{
    UNIFORM,
    STORAGE
};

class GPUSharedBufferData
{
public:
	GPUSharedBufferData() = default;
	GPUSharedBufferData(GPUSharedBufferType gpuSharedBufferType, const std::vector<GPUVariableDefinitionData>& gpuVariableDefinitionDataArray, const std::string& blockName):
        mType(gpuSharedBufferType), mGPUVariableDefinitionDataArray(gpuVariableDefinitionDataArray), mBufferName(blockName) {}
	GPUSharedBufferData(GPUSharedBufferType gpuSharedBufferType, const std::vector<GPUVariableDefinitionData>& gpuVariableDefinitionDataArray, const std::string& blockName, const std::string& instanceName):
        mType(gpuSharedBufferType), mGPUVariableDefinitionDataArray(gpuVariableDefinitionDataArray), mBufferName(blockName), mInstanceName(instanceName) {}

public:
    GPUSharedBufferType mType = GPUSharedBufferType::UNIFORM;
    std::vector<GPUVariableDefinitionData> mGPUVariableDefinitionDataArray;
    std::string mBufferName;
    std::string mInstanceName;

public:
    GPUVariableData getScopedGPUVariableData(u32 i) const;
};

class GPUSharedBuffer
{
public:
	GPUSharedBuffer() = default;

    void init(u32 bindingPoint, const GPUSharedBufferData& gpuBlockData, bool isStatic);
    void resize(u32 size);
    template <class T>
    void setData(const T& data)
    {
        switch (mGPUSharedBufferData.mType)
        {
        case UNIFORM:
            GET_SYSTEM(GPUInterface).setDataUBOAnyStruct(mBufferId, data);
            break;
        case STORAGE:
            GET_SYSTEM(GPUInterface).setDataSSBOAnyStruct(mBufferId, data);
            break;
        }
    }
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
        switch (mGPUSharedBufferData.mType)
        {
        case UNIFORM:
            GET_SYSTEM(GPUInterface).setDataUBOAnyType(mBufferId, data);
            break;
        case STORAGE:
            GET_SYSTEM(GPUInterface).setDataSSBOAnyType(mBufferId, data);
            break;
        }
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