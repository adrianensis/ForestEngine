#pragma once

#include "Graphics/GPU/GPUVariable.hpp"
#include "Graphics/GPU/Buffer/GPUBuffer.h"
// #include "Graphics/GPU/GPUInstance.hpp"

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
    void init(Ptr<GPUContext> gpuContext, u32 size, u32 bindingPoint, const GPUUniformBufferData& gpuBufferData, bool isStatic);
    void createBuffer();
//     template <class T>
//     void resize(u32 size)
//     {
//         resizeBytes(size * sizeof(T));
//     }
//     void resizeBytes(u32 bytes)
//     {
//         checkMaxSize(bytes);
// //        GET_SYSTEM(GPUInterface).resizeBuffer(mGPUUniformBufferData.mType, mBufferId, 1, bytes, mIsStatic);
//     }
    template <class T>
    void setData(const T& data)
    {
        checkMaxSize(sizeof(T));
//        // GET_SYSTEM(GPUInterface).setBufferData(mGPUUniformBufferData.mType, mBufferId, data);
        // const GPUUniformBuffer& uniformBuffer = uniformBuffers[GET_SYSTEM(GPUInstance).currentFrame];
        // uniformBuffer.setData((void*) data);
    }
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
        checkMaxSize(sizeof(T) * data.size());
//        // GET_SYSTEM(GPUInterface).setBufferDataArray(mGPUUniformBufferData.mType, mBufferId, data);
        // const GPUUniformBuffer& uniformBuffer = uniformBuffers[GET_SYSTEM(GPUInstance).currentFrame];
        buffer.setData((void*) data.data());
    }
    void setDataArray(const ByteBuffer& data)
    {
//	    // GET_SYSTEM(GPUInterface).setBufferDataArray(mGPUUniformBufferData.mType, mBufferId, data);
        // const GPUUniformBuffer& uniformBuffer = uniformBuffers[GET_SYSTEM(GPUInstance).currentFrame];
        // uniformBuffer.setData((void*) data.getBuffer().data());
    }
    void terminate();

    const GPUBuffer& getBuffer() const;
    void setData(void* data) const;

private:
    bool initialize();
    void checkMaxSize(u32 bytes) const;

private:
	u32 mBindingPoint = 0;
    GPUUniformBufferData mGPUUniformBufferData;
	u32 mBufferId = 0;
    bool mIsStatic = false;
    void* mGPUPointer = nullptr;

    Ptr<GPUContext> mGPUContext;
    GPUBuffer buffer;
    u32 mSize = 0;

public:
    GET(BindingPoint)
    GET(Size)
    CRGET(GPUUniformBufferData)
};