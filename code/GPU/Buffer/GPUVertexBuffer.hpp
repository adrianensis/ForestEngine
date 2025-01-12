#pragma once

#include "GPU/Shader/GPUVariable.hpp"

#include "GPU/Core/GPUPhysicalDevice.h"
#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUCommandPool.h"
#include "GPU/Buffer/GPUBuffer.h"

class GPUVertexBufferData
{
public:
	GPUVertexBufferData() = default;
    GPUVertexBufferData(const GPUVariableData& gpuVariableData): mGPUVariableData(gpuVariableData) {};
    GPUVertexBufferData(const GPUVariableData& gpuVariableData, u32 instanceDivisor): mGPUVariableData(gpuVariableData), mInstanceDivisor(instanceDivisor) {};
    
public:
    GPUVariableData mGPUVariableData;
    u32 mInstanceDivisor = 0;
};

class GPUVertexBuffer
{
public:
    void init(WeakPtr<GPUContext> gpuContext, u32 attributeLocation, const GPUVertexBufferData& data, u32 size, bool isStatic);
    void resize(u32 size);
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
        if (!setData((const void*)data.data(), data.size() * mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes))
        {
            CHECK_MSG(false, "Could not initialize Vulkan vertex buffer");
        }
    }
    void setDataArray(const ByteBuffer& data)
    {
        if (!setData((const void*)data.getBuffer().data(), data.getBuffer().size()))
        {
            CHECK_MSG(false, "Could not initialize Vulkan vertex buffer");
        }
    }
    u32 getAttributeLocation() const;
    u32 getAttributeLocationWithOffset() const;
    void terminate();

    const GPUBuffer& getGPUBuffer() const;
    bool setData(const void* data, u32 size);

public:
    GPUVertexBufferData mData;
private:
    u32 mAttributeLocation = 0;
    u32 mAttributeOffset = 0;
    u32 mPreviousOffsetInBytes = 0;
    bool mIsStatic = false;

    WeakPtr<GPUContext> mGPUContext;
    GPUBuffer buffer;
};