#pragma once

#include "Graphics/GPU/GPUInterface.hpp"

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
    UNIFORM, "uniform",
);

enum class GPUPrimitiveType : u32
{
    STRUCT = 0,
    INT = GL_INT,
    FLOAT = GL_FLOAT,
    BOOL = GL_BOOL,
};

class GPUDataType
{
public:
    std::string mName;
    u32 mTypeSizeInBytes = 0;
    GPUPrimitiveType mPrimitiveType = GPUPrimitiveType::FLOAT;

    u32 getPrimitiveTypeSizeInBytes() const
    {
        u32 primitiveTypeSizeInBytes = 0;
        switch (mPrimitiveType)
        {
            case GPUPrimitiveType::FLOAT:
                primitiveTypeSizeInBytes = sizeof(f32);
            break;
            case GPUPrimitiveType::INT:
                primitiveTypeSizeInBytes = sizeof(i32);
            break;
            case GPUPrimitiveType::BOOL:
                primitiveTypeSizeInBytes = sizeof(bool);
            break;
            case GPUPrimitiveType::STRUCT:
                primitiveTypeSizeInBytes = mTypeSizeInBytes;
            break;
        }

        return primitiveTypeSizeInBytes;
    }

    u32 getSizePrimitiveType() const
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
    GPUVariableData(GPUStorage gpuStorage, const GPUDataType& gpuDataType, const std::string& name):
    mGPUStorage(gpuStorage), mGPUDataType(gpuDataType), mName(name)
    {}
    GPUVariableData(GPUStorage gpuStorage, const GPUVariableData& otherGPUVariableData):
    mGPUStorage(gpuStorage), mGPUDataType(otherGPUVariableData.mGPUDataType), mName(otherGPUVariableData.mName)
    {}

    GPUStorage mGPUStorage = GPUStorage::NONE;
    GPUDataType mGPUDataType;
    std::string mName;
};

class GPUVariableDefinitionData: public GPUVariableData
{
public:
    GPUVariableDefinitionData() = default;
    GPUVariableDefinitionData(GPUStorage gpuStorage, const GPUDataType& gpuDataType, const std::string& name):
    GPUVariableData(gpuStorage, gpuDataType, name)
    {}
    GPUVariableDefinitionData(const GPUVariableData& gpuVariableData):
    GPUVariableData(gpuVariableData)
    {}   
    GPUVariableDefinitionData(const GPUVariableData& gpuVariableData, const std::string& value):
    GPUVariableData(gpuVariableData), mValue(value)
    {}   
    GPUVariableDefinitionData(const GPUVariableData& gpuVariableData, const std::string& value, const std::string& arraySize):
    GPUVariableData(gpuVariableData), mValue(value), mArraySize(arraySize)
    {}
    GPUVariableDefinitionData(GPUStorage gpuStorage, const GPUVariableData& gpuVariableData):
    GPUVariableData(gpuStorage, gpuVariableData)
    {}   
    std::string mValue;
    std::string mArraySize;
};

class GPUStructDefinition
{
public:
    class GPUStructVariable
    {
    public:
        GPUDataType mGPUDataType;
        std::string mName;
    };

    std::string mName;
    std::vector<GPUStructVariable> mPrimitiveVariables;

    GPUStructDefinition() = default;
    GPUStructDefinition(const std::string& name, const std::vector<GPUStructVariable>& primitiveVariables):
        mName(name), mPrimitiveVariables(primitiveVariables){}

    u32 getTypeSizeInBytes() const
    {
        u32 typeSizeInBytes = 0;
        FOR_ARRAY(i, mPrimitiveVariables)
        {
            typeSizeInBytes += mPrimitiveVariables[i].mGPUDataType.mTypeSizeInBytes;
        }

        return typeSizeInBytes;
    }
};