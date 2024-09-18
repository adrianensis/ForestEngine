#pragma once

#include "Graphics/GPU/GPUVariable.hpp"

#include "Graphics/GPU/Core/GPUPhysicalDevice.h"
#include "Graphics/GPU/Core/GPUDevice.h"
#include "Graphics/GPU/Core/GPUCommandPool.h"
#include "Graphics/GPU/Buffer/GPUBuffer.h"

class GPUVertexBufferData
{
public:
	GPUVertexBufferData() = default;
    GPUVertexBufferData(const GPUVariableData& gpuVariableData): mGPUVariableData(gpuVariableData) {};
    GPUVertexBufferData(const GPUVariableData& gpuVariableData, u32 instanceDivisor): mGPUVariableData(gpuVariableData), mInstanceDivisor(instanceDivisor) {};
    
public:
    GPUVariableData mGPUVariableData;
    // glVertexAttribDivisor — modify the rate at which generic vertex attributes advance during instanced rendering
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribDivisor.xhtml
    u32 mInstanceDivisor = 0;
};

class GPUVertexBuffer
{
public:
    void init(Ptr<GPUContext> gpuContext, u32 attributeLocation, const GPUVertexBufferData& data, bool isStatic);
    void createBuffer();
    void resize(u32 size);
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
        // if (!vulkanVertexBuffer->initialize(data))
        {
            // CHECK_MSG(false, "Could not initialize Vulkan vertex buffer");
        }
//	    // GET_SYSTEM(GPUInterface).setBufferDataArray(GPUBufferType::VERTEX, mBufferId, data);
    }
    void setDataArray(const ByteBuffer& data)
    {
        if (!setData((void*)data.getBuffer().data(), data.getBuffer().size()))
        {
            CHECK_MSG(false, "Could not initialize Vulkan vertex buffer");
        }
    }
    u32 getAttributeLocation() const;
    u32 getAttributeLocationWithOffset() const;
    void terminate();

    const GPUBuffer& getGPUBuffer() const;
    bool setData(void* data, u32 size);

public:
    GPUVertexBufferData mData;
private:
	u32 mBufferId = 0;
    u32 mAttributeLocation = 0;
    u32 mAttributeOffset = 0;
    u32 mPreviousOffsetInBytes = 0;
    bool mIsStatic = false;

    Ptr<GPUContext> mGPUContext;
    GPUBuffer buffer;

public:
    GET(BufferId)
};