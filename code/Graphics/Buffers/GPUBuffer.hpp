#pragma once

#include "Core/Module.hpp"

/*
    - none: (default) local read/write memory,
    or input parameter

    - const: global compile-time constant, or
    read-only function parameter,
    or read-only local variable

    - in: linkage into shader from previous stage

    - out: linkage out of a shader to next stage

    - attribute: same as in for vertex shader

    - uniform: linkage between a shader, OpenGL,
    and the application

    - varying: same as in for vertex shader, same as
    out for fragment shader
*/
DECLARE_ENUM(GPUStorage,
    NONE, "none",
    IN, "in",
    OUT, "out",
    VARYING, "varying",
    ATTRIBUTE, "attribute",
    CONST, "const",
    UNIFORM, "uniform"
);

enum class GPUBufferPrimitiveType : u32
{
    INT = GL_INT,
    FLOAT = GL_FLOAT,
    BOOL = GL_BOOL,
};

class GPUDataType
{
public:
    std::string mTypeName;
    u32 mTypeSizeInBytes;
    GPUBufferPrimitiveType mPrimitiveType = GPUBufferPrimitiveType::FLOAT;

    u32 getPrimitiveTypeSizeInBytes()
    {
        u32 primitiveTypeSize = 0;
        switch (mPrimitiveType)
        {
            case GPUBufferPrimitiveType::FLOAT:
                primitiveTypeSize = sizeof(f32);
            break;
            case GPUBufferPrimitiveType::INT:
                primitiveTypeSize = sizeof(i32);
            break;
            case GPUBufferPrimitiveType::BOOL:
                primitiveTypeSize = sizeof(bool);
            break;
        }

        return primitiveTypeSize;
    }

    u32 getSizePrimitiveType()
    {
        u32 primitiveTypeSizeInBytes = getPrimitiveTypeSizeInBytes();
        u32 sizeInPrimitiveTypes = mTypeSizeInBytes/primitiveTypeSizeInBytes;
        return sizeInPrimitiveTypes;
    }
};

class GPUVariableData
{
public:
    GPUVariableData() = default;
    GPUVariableData(GPUDataType gpuDataType, std::string name, std::string value, std::string arraySize):
    mGPUDataType(gpuDataType),
    mName(name),
    mValue(value),
    mArraySize(arraySize)
    {}
    GPUVariableData(GPUDataType gpuDataType, std::string name, std::string value):
    mGPUDataType(gpuDataType),
    mName(name),
    mValue(value)
    {}
    GPUVariableData(GPUDataType gpuDataType, std::string name):
    mGPUDataType(gpuDataType),
    mName(name)
    {}

    GPUDataType mGPUDataType;
    std::string mName;
    std::string mValue;
    std::string mArraySize;
};

class GPUAttributeData : public GPUVariableData
{
public:
    GPUAttributeData() = default;
    GPUAttributeData(GPUStorage gpuStorage, const GPUVariableData& gpuVariableData):
    GPUVariableData(gpuVariableData),
    mGPUStorage(gpuStorage)
    {}

    GPUStorage mGPUStorage = GPUStorage::NONE;
};

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
    u32 attribute(GPUBufferPrimitiveType primitiveType, u32 customSizeInPrimitiveTypes = 0);

public:
    GPUBufferData mData;
private:
	u32 mVBO = 0; // TODO: change u32 for GLuint
    u32 mAttributeLocation = 0;
    u32 mAttributeOffset = 0;
    u32 mPreviousOffsetInBytes = 0;
    bool mIsStatic = false;
};