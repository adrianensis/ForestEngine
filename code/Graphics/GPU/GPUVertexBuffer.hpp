#pragma once

#include "Graphics/GPU/GPUVariable.hpp"
#include "Graphics/GPU/GPUInterface.hpp"

class GPUVertexBufferData
{
public:
	GPUVertexBufferData() = default;
    GPUVertexBufferData(const GPUVariableData& gpuVariableData): mGPUVariableData(gpuVariableData) {};
    
public:
    // glVertexAttribDivisor — modify the rate at which generic vertex attributes advance during instanced rendering
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribDivisor.xhtml
    u32 mInstanceDivisor = 0;
    GPUVariableData mGPUVariableData;
};

class GPUVertexBuffer
{
public:
	GPUVertexBuffer() = default;

    void init(u32 attributeLocation, const GPUVertexBufferData& data, bool isStatic);
    void resize(u32 size);
    template <class T>
    void setData(const std::vector<T>& data)
    {
	    GET_SYSTEM(GPUInterface).setDataVBOAnyType<T>(mVBO, data);
    }
    u32 getAttributeLocation() const;
    u32 getAttributeLocationWithOffset() const;
    void terminate();

private:
    u32 attribute(GPUPrimitiveType primitiveType);

public:
    GPUVertexBufferData mData;
private:
	u32 mVBO = 0; // TODO: change u32 for GLuint
    u32 mAttributeLocation = 0;
    u32 mAttributeOffset = 0;
    u32 mPreviousOffsetInBytes = 0;
    bool mIsStatic = false;
};