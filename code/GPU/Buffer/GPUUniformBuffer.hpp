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
    // void resize(GPU::u32 size);
    template <class T>
    void setData(const T& data)
    {
        CHECK_MSG(sizeof(T) <= mSize, "Data size {} exceeds Uniform Buffer capacity {}!", sizeof(T), mSize);
        mBuffers[mGPUContext->currentFrame].setData((const void*) &data, sizeof(T));
    }
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
        size_t totalSize = sizeof(T) * data.size();
        CHECK_MSG(totalSize <= mSize, "Data array size {} exceeds Uniform Buffer capacity {}!", totalSize, mSize);
        mBuffers[mGPUContext->currentFrame].setData((const void*) data.data(), totalSize);
    }
    void setDataArray(const GPU::ByteBuffer& data)
    {
        CHECK_MSG(data.sizeInBytes() <= mSize, "Byte buffer size {} exceeds Uniform Buffer capacity {}!", data.sizeInBytes(), mSize);
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
    auto getBindingPoint() const { return mBindingPoint; }
    auto getSize() const { return mSize; }
    const auto& getGPUUniformBufferData() const { return mGPUUniformBufferData; }
    const auto& getBuffers() const { return mBuffers; }
};