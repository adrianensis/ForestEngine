#pragma once

#include "Graphics/GPU/GPUVariable.hpp"
#include "Graphics/GPU/GPUInterface.hpp"

class GPUSharedBufferData
{
public:
	GPUSharedBufferData() = default;
    GPUSharedBufferData(const GPUVariableData& gpuVariableData): mGPUVariableData(gpuVariableData) {};
    
public:
    GPUVariableData mGPUVariableData;
};

class GPUSharedBuffer
{
public:
	GPUSharedBuffer() = default;

    void init(const GPUSharedBufferData& data, bool isStatic);
    void resize(u32 size);
    template <class T>
    void setData(const std::vector<T>& data)
    {
	    GET_SYSTEM(GPUInterface).setDataUBOAnyType<T>(mUBO, data);
    }
    void terminate();

public:
    GPUSharedBufferData mData;
    
private:
	u32 mUBO = 0; // TODO: change u32 for GLuint
    bool mIsStatic = false;
};