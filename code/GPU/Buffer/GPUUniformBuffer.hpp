#pragma once

#include "GPU/Shader/GPUVariable.hpp"
#include "GPU/Buffer/GPUBuffer.h"

class GPUUniformBufferData
{
public:
	GPUUniformBufferData() = default;
	GPUUniformBufferData(GPUBufferType gpuUniformBufferType, const std::vector<GPUVariableDefinitionData>& gpuVariableDefinitionDataArray, HashedString blockName, HashedString instanceName):
        mType(gpuUniformBufferType), mGPUVariableDefinitionDataArray(gpuVariableDefinitionDataArray), mBufferName(blockName), mInstanceName(instanceName) {}

    GPUVariableData getScopedGPUVariableData(u32 i) const;

public:
    GPUBufferType mType = GPUBufferType::UNIFORM;
    std::vector<GPUVariableDefinitionData> mGPUVariableDefinitionDataArray;
    HashedString mBufferName;
    HashedString mInstanceName;
};

class GPUUniformBuffer
{
public:
    void init(WeakPtr<GPUContext> gpuContext, u32 size, u32 bindingPoint, const GPUUniformBufferData& gpuUniformBufferData, bool isStatic);
    template <class T>
    void setData(const T& data)
    {
        mBuffer.setData((const void*) &data);
    }
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
        mBuffer.setData((const void*) data.data());
    }
    void setDataArray(const ByteBuffer& data)
    {
        mBuffer.setData((const void*) data.getBuffer().data());
    }
    void terminate();

    void setData(const void* data) const;

private:
	u32 mBindingPoint = 0;
    GPUUniformBufferData mGPUUniformBufferData;
    bool mIsStatic = false;
    void* mGPUPointer = nullptr;

    WeakPtr<GPUContext> mGPUContext;
    GPUBuffer mBuffer;
    u32 mSize = 0;

public:
    GET(BindingPoint)
    GET(Size)
    CRGET(GPUUniformBufferData)
    CRGET(Buffer)
};