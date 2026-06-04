#pragma once

#include "Core/CoreBase.hpp"
#include "GPU/Core/GPUContext.hpp"
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
        onUpdate(mGPUContext->currentFrame);
    }
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
        mBuffers[mGPUContext->currentFrame].setData((const void*) data.data(), sizeof(T) * data.size());
        onUpdate(mGPUContext->currentFrame);
    }
    void setDataArray(const Core::ByteBuffer& data)
    {
        mBuffers[mGPUContext->currentFrame].setData((const void*) data.getBuffer().data(), data.sizeInBytes());
        onUpdate(mGPUContext->currentFrame);
    }
    void terminate();

    bool needUpdate() const
    {
        return needUpdateFrame(mGPUContext->currentFrame);
    }

    bool needUpdateFrame(GPU::u32 frame) const
    {
        return mBuffersUpdateRequest[frame];
    }

private:
    void onUpdate(GPU::u32 frame)
    {
        mBuffersUpdateRequest[frame] = false;
        if((frame + 1) < GPUContext::MAX_FRAMES_IN_FLIGHT) // (frame + 1) because arrays start at 0!
        {
            // request update next buffer in next frame
            mBuffersUpdateRequest[(frame + 1) % GPUContext::MAX_FRAMES_IN_FLIGHT] = true;
        }
    }

private:
	GPU::u32 mBindingPoint = 0;
    GPUUniformBufferData mGPUUniformBufferData;
    bool mIsStatic = false;

    GPUContext* mGPUContext = nullptr;
    GPUBuffer mBuffers[GPUContext::MAX_FRAMES_IN_FLIGHT] = {};
    bool mBuffersUpdateRequest[GPUContext::MAX_FRAMES_IN_FLIGHT] = {};
    GPU::u32 mSize = 0;

public:
    GET(BindingPoint)
    GET(Size)
    CRGET(GPUUniformBufferData)
    CRGET(Buffers)
};