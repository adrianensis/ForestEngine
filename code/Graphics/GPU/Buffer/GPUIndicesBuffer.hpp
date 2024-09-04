#pragma once

#include "Graphics/GPU/GPUVariable.hpp"
#include "Graphics/GPU/Buffer/GPUBuffer.h"

class GPUIndicesBuffer
{
public:
    void init(Ptr<GPUContext> gpuContext, const GPUDataType& gpuDataType, bool isStatic);
    void resize(u32 size);
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
//	    GET_SYSTEM(GPUInterface).setBufferDataArray(GPUBufferType::INDEX, mBufferId, data);
    }
    void setDataArray(const ByteBuffer& data)
    {
//	    GET_SYSTEM(GPUInterface).setBufferDataArray(GPUBufferType::INDEX, mBufferId, data);
    }
    void terminate();

    const GPUBuffer& getGPUBuffer() const;
    bool initialize(const std::vector<uint32_t>& indices);

private:
	u32 mBufferId = 0;
    GPUDataType mGPUDataType;
    bool mIsStatic = false;

    
    Ptr<GPUContext> mGPUContext;
    GPUBuffer buffer;

public:
    GET(BufferId)
};