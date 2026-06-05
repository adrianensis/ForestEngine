#pragma once

#include "Core/CoreBase.hpp"
#include "Core/CoreMacros.hpp"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Core/GPUDefinitions.h"
#include "GPU/Shader/GPUVariable.hpp"
#include "GPU/Buffer/GPUBuffer.h"
#include "GPU/Core/GPUByteBuffer.hpp"

class GPUUniformBufferData
{
public:
	GPUUniformBufferData() = default;
	GPUUniformBufferData(GPUBufferType gpuUniformBufferType,
        const std::vector<GPUVariableDefinitionData>& gpuVariableDefinitionDataArray,
        std::string blockName, std::string instanceName,
        GPUDescriptorSetScope descriptorSetScope):
        mType(gpuUniformBufferType), mGPUVariableDefinitionDataArray(gpuVariableDefinitionDataArray),
        mBufferName(blockName), mInstanceName(instanceName),
        mDescriptorSetScope(descriptorSetScope) {}

    GPUVariableData getScopedGPUVariableData(GPU::u32 i) const;

public:
    GPUBufferType mType = GPUBufferType::UNIFORM;
    std::vector<GPUVariableDefinitionData> mGPUVariableDefinitionDataArray;
    std::string mBufferName;
    std::string mInstanceName;
    GPUDescriptorSetScope mDescriptorSetScope = GPUDescriptorSetScope::LOCAL;
};

class GPUUniformBuffer
{
public:
    void init(GPUContext* gpuContext, GPU::u32 size, GPU::u32 bindingPoint, const GPUUniformBufferData& gpuUniformBufferData, bool isStatic);
    void resize(GPU::u32 size);
    template <class T>
    void setData(const T& data)
    {
        mBuffers[mGPUContext->currentFrame].setData((const void*) &data, sizeof(T));
    }
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
        mBuffers[mGPUContext->currentFrame].setData((const void*) data.data(), sizeof(T) * data.size());
    }
    void setDataArray(const GPU::ByteBuffer& data)
    {
        mBuffers[mGPUContext->currentFrame].setData((const void*) data.getBuffer().data(), data.sizeInBytes());
    }
    void terminate();

private:


private:
	GPU::u32 mBindingPoint = 0;
    GPUUniformBufferData mGPUUniformBufferData;
    bool mIsStatic = false;

    GPUContext* mGPUContext = nullptr;
    GPUBuffer mBuffers[GPUContext::MAX_FRAMES_IN_FLIGHT] = {};
    GPU::u32 mSize = 0;

public:
    GET(BindingPoint)
    GET(Size)
    CRGET(GPUUniformBufferData)
    CRGET(Buffers)
};