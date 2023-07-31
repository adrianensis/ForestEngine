#pragma once

#include "Graphics/GPU/GPUAttribute.hpp"

class GPUBufferData
{
public:
	GPUBufferData() = default;
    GPUBufferData(const GPUAttributeData& gpuAttributeData): mGPUAttributeData(gpuAttributeData) {};
    
public:
    // glVertexAttribDivisor — modify the rate at which generic vertex attributes advance during instanced rendering
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribDivisor.xhtml
    u32 mInstanceDivisor = 0;
    u32 mAttributeDivisorSizeInPrimitiveTypes = 0;
    GPUAttributeData mGPUAttributeData;
};

class GPUBuffer
{
public:
	GPUBuffer() = default;

    void init(u32 attributeLocation, const GPUBufferData& data, bool isStatic);
    void resize(u32 size);
    template <class T>
    void setData(const std::vector<T>& data)
    {
	    GET_SYSTEM(RenderContext).setDataVBOAnyType<T>(mVBO, data);
    }
    u32 getAttributeLocation() const;
    u32 getAttributeLocationWithOffset() const;
    void terminate();

private:
    u32 attribute(GPUPrimitiveType primitiveType, u32 customSizeInPrimitiveTypes = 0);

public:
    GPUBufferData mData;
private:
	u32 mVBO = 0; // TODO: change u32 for GLuint
    u32 mAttributeLocation = 0;
    u32 mAttributeOffset = 0;
    u32 mPreviousOffsetInBytes = 0;
    bool mIsStatic = false;
};