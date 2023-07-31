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

enum class GPUPrimitiveType : u32
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
    GPUPrimitiveType mPrimitiveType = GPUPrimitiveType::FLOAT;

    u32 getPrimitiveTypeSizeInBytes()
    {
        u32 primitiveTypeSize = 0;
        switch (mPrimitiveType)
        {
            case GPUPrimitiveType::FLOAT:
                primitiveTypeSize = sizeof(f32);
            break;
            case GPUPrimitiveType::INT:
                primitiveTypeSize = sizeof(i32);
            break;
            case GPUPrimitiveType::BOOL:
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
