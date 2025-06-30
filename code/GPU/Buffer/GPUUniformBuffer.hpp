#pragma once

#include "GPU/Shader/GPUVariable.hpp"
#include "GPU/Buffer/GPUBuffer.h"

class GPUUniformBufferData
{
public:
	GPUUniformBufferData() = default;
	GPUUniformBufferData(GPUBufferType gpuUniformBufferType, const std::vector<GPUVariableDefinitionData>& gpuVariableDefinitionDataArray, Core::HashedString blockName, Core::HashedString instanceName):
        mType(gpuUniformBufferType), mGPUVariableDefinitionDataArray(gpuVariableDefinitionDataArray), mBufferName(blockName), mInstanceName(instanceName) {}

    GPUVariableData getScopedGPUVariableData(u32 i) const;

public:
    GPUBufferType mType = GPUBufferType::UNIFORM;
    std::vector<GPUVariableDefinitionData> mGPUVariableDefinitionDataArray;
    Core::HashedString mBufferName;
    Core::HashedString mInstanceName;
};

class GPUUniformBuffer
{
public:
    void init(Ptr<GPUContext> gpuContext, u32 size, u32 bindingPoint, const GPUUniformBufferData& gpuUniformBufferData, bool isStatic);
    void resize(u32 size);
    template <class T>
    void setData(const T& data)
    {
        mBuffer.setData((const void*) &data, sizeof(T));
    }
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
        mBuffer.setData((const void*) data.data(), sizeof(T) * data.size());
    }
    void setDataArray(const ByteBuffer& data)
    {
        mBuffer.setData((const void*) data.getBuffer().data(), data.sizeInBytes());
    }
    void terminate();

private:
	u32 mBindingPoint = 0;
    GPUUniformBufferData mGPUUniformBufferData;
    bool mIsStatic = false;
    void* mGPUPointer = nullptr;

    Ptr<GPUContext> mGPUContext;
    GPUBuffer mBuffer;
    u32 mSize = 0;

public:
    GET(BindingPoint)
    GET(Size)
    CRGET(GPUUniformBufferData)
    CRGET(Buffer)
};