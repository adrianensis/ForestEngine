#pragma once

#include "Graphics/GPU/GPUVariable.hpp"
#include "Graphics/GPU/GPUInterface.hpp"

class GPUSharedBufferData
{
public:
	GPUSharedBufferData() = default;
	GPUSharedBufferData(GPUBufferType gpuSharedBufferType, const std::vector<GPUVariableDefinitionData>& gpuVariableDefinitionDataArray, HashedString blockName, HashedString instanceName):
        mType(gpuSharedBufferType), mGPUVariableDefinitionDataArray(gpuVariableDefinitionDataArray), mBufferName(blockName), mInstanceName(instanceName) {}

    GPUVariableData getScopedGPUVariableData(u32 i) const;

public:
    GPUBufferType mType = GPUBufferType::UNIFORM;
    std::vector<GPUVariableDefinitionData> mGPUVariableDefinitionDataArray;
    HashedString mBufferName;
    HashedString mInstanceName;
};

class GPUSharedBuffer
{
public:
	GPUSharedBuffer() = default;

    void init(u32 bindingPoint, const GPUSharedBufferData& gpuBufferData, bool isStatic);
    void createBuffer();
    template <class T>
    void resize(u32 size)
    {
        resizeBytes(size * sizeof(T));
    }
    void resizeBytes(u32 bytes)
    {
        checkMaxSize(bytes);
        GET_SYSTEM(GPUInterface).resizeBuffer(mGPUSharedBufferData.mType, mBufferId, 1, bytes, mIsStatic);
    }
    template <class T>
    void setData(const T& data)
    {
        checkMaxSize(sizeof(T));
        GET_SYSTEM(GPUInterface).setBufferData(mGPUSharedBufferData.mType, mBufferId, data);
    }
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
        checkMaxSize(sizeof(T) * data.size());
        GET_SYSTEM(GPUInterface).setBufferDataArray(mGPUSharedBufferData.mType, mBufferId, data);
    }
    void setDataArray(const ByteBuffer& data)
    {
	    GET_SYSTEM(GPUInterface).setBufferDataArray(mGPUSharedBufferData.mType, mBufferId, data);
    }
    void terminate();

private:
    void checkMaxSize(u32 bytes) const;

private:
	u32 mBindingPoint = 0;
    GPUSharedBufferData mGPUSharedBufferData;
	u32 mBufferId = 0;
    bool mIsStatic = false;
    void* mGPUPointer = nullptr;

public:
    GET(BindingPoint)
    CRGET(GPUSharedBufferData)
    GET(BufferId)
};