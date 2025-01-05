#pragma once

#include "GPU/Shader/GPUVariable.hpp"
#include "GPU/Buffer/GPUBuffer.h"

class GPUIndicesBuffer
{
public:
    void init(WeakPtr<GPUContext> gpuContext, const GPUDataType& gpuDataType, bool isStatic);
    void resize(u32 size);
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
        if (!setData((const void*)data.data(), data.size() * mGPUDataType.mTypeSizeInBytes))
        {
            CHECK_MSG(false, "Could not initialize Vulkan indices buffer");
        }
    }
    void setDataArray(const ByteBuffer& data)
    {
        if (!setData((const void*)data.getBuffer().data(), data.getBuffer().size()))
        {
            CHECK_MSG(false, "Could not initialize Vulkan indices buffer");
        }
    }
    void terminate();

    const GPUBuffer& getGPUBuffer() const;
    bool setData(const void* data, u32 size);

private:
	u32 mBufferId = 0;
    GPUDataType mGPUDataType;
    bool mIsStatic = false;

    
    WeakPtr<GPUContext> mGPUContext;
    GPUBuffer buffer;

public:
    GET(BufferId)
};