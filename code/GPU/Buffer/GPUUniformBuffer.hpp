#pragma once

#include "GPU/Core/GPUDefinitions.h"
#include "GPU/Shader/GPUVariable.hpp"
#include "GPU/Buffer/GPUBuffer.h"
#include "Core/Memory/ByteBuffer.hpp"

class GPUUniformBufferData
{
public:
	GPUUniformBufferData() = default;
	GPUUniformBufferData(GPUBufferType gpuUniformBufferType,
        const std::vector<GPUVariableDefinitionData>& gpuVariableDefinitionDataArray,
        Core::HashedString blockName, Core::HashedString instanceName,
        GPUDescriptorSetScope descriptorSetScope):
        mType(gpuUniformBufferType), mGPUVariableDefinitionDataArray(gpuVariableDefinitionDataArray),
        mBufferName(blockName), mInstanceName(instanceName),
        mDescriptorSetScope(descriptorSetScope) {}

    GPUVariableData getScopedGPUVariableData(Core::u32 i) const;

public:
    GPUBufferType mType = GPUBufferType::UNIFORM;
    std::vector<GPUVariableDefinitionData> mGPUVariableDefinitionDataArray;
    Core::HashedString mBufferName;
    Core::HashedString mInstanceName;
    GPUDescriptorSetScope mDescriptorSetScope = GPUDescriptorSetScope::LOCAL;
};

class GPUUniformBuffer
{
public:
    void init(GPUContext* gpuContext, Core::u32 size, Core::u32 bindingPoint, const GPUUniformBufferData& gpuUniformBufferData, bool isStatic);
    void resize(Core::u32 size);
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
    void setDataArray(const Core::ByteBuffer& data)
    {
        mBuffer.setData((const void*) data.getBuffer().data(), data.sizeInBytes());
    }
    void terminate();

private:
	Core::u32 mBindingPoint = 0;
    GPUUniformBufferData mGPUUniformBufferData;
    bool mIsStatic = false;

    GPUContext* mGPUContext = nullptr;
    GPUBuffer mBuffer;
    Core::u32 mSize = 0;

public:
    GET(BindingPoint)
    GET(Size)
    CRGET(GPUUniformBufferData)
    CRGET(Buffer)
};