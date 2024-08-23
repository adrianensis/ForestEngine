#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/GPU/Vulkan.h"

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

DECLARE_ENUM(GPUInterpolation,
    NONE, "",
    FLAT, "flat",
    NOPERSPECTIVE, "noperspective",
    SMOOTH, "smooth"
);

class GPUDataType
{
public:
    HashedString mName;
    u32 mTypeSizeInBytes = 0;
    GPUPrimitiveDataType mPrimitiveDataType = GPUPrimitiveDataType::FLOAT;

    u32 getPrimitiveTypeSizeInBytes() const
    {
        u32 primitiveTypeSizeInBytes = 0;
        switch (mPrimitiveDataType)
        {
            case GPUPrimitiveDataType::FLOAT:
                primitiveTypeSizeInBytes = sizeof(f32);
            break;
            case GPUPrimitiveDataType::INT:
                primitiveTypeSizeInBytes = sizeof(i32);
            break;
            case GPUPrimitiveDataType::BOOL:
                primitiveTypeSizeInBytes = sizeof(bool);
            break;
            case GPUPrimitiveDataType::STRUCT:
                primitiveTypeSizeInBytes = mTypeSizeInBytes;
            break;
            case GPUPrimitiveDataType::VOID:
                CHECK_MSG(false, "GPU Variable cannot be void!");
            break;
            default:
                CHECK_MSG(false, "Primitive Data type not supported for GPU Variable!");
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
    GPUVariableData(GPUStorage gpuStorage, const GPUDataType& gpuDataType, const HashedString& name):
    mGPUStorage(gpuStorage), mGPUDataType(gpuDataType), mName(name)
    {}
    GPUVariableData(GPUStorage gpuStorage, const GPUVariableData& otherGPUVariableData):
    mGPUStorage(gpuStorage), mGPUDataType(otherGPUVariableData.mGPUDataType), mName(otherGPUVariableData.mName)
    {}

    GPUStorage mGPUStorage = GPUStorage::NONE;
    GPUDataType mGPUDataType;
    HashedString mName;
};

class GPUVariableDefinitionData: public GPUVariableData
{
public:
    GPUVariableDefinitionData() = default;
    GPUVariableDefinitionData(GPUInterpolation gpuInterpolation, GPUStorage gpuStorage, const GPUDataType& gpuDataType, const HashedString& name):
    GPUVariableData(gpuStorage, gpuDataType, name), mGPUInterpolation(gpuInterpolation)
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
    GPUVariableDefinitionData(GPUStorage gpuStorage, const GPUDataType& gpuDataType, const HashedString& name):
    GPUVariableData(gpuStorage, gpuDataType, name)
    {}
    GPUVariableDefinitionData(GPUInterpolation gpuInterpolation, const GPUVariableData& gpuVariableData, const std::string& value, const std::string& arraySize):
    GPUVariableData(gpuVariableData), mGPUInterpolation(gpuInterpolation), mValue(value), mArraySize(arraySize)
    {}
    GPUVariableDefinitionData(GPUInterpolation gpuInterpolation, GPUStorage gpuStorage, const GPUVariableData& gpuVariableData):
    GPUVariableData(gpuStorage, gpuVariableData), mGPUInterpolation(gpuInterpolation)
    {}

    GPUInterpolation mGPUInterpolation = GPUInterpolation::NONE;
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
        HashedString mName;
    };

    HashedString mName;
    std::vector<GPUStructVariable> mPrimitiveVariables;

    GPUStructDefinition() = default;
    GPUStructDefinition(const HashedString& name, const std::vector<GPUStructVariable>& primitiveVariables):
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

class GPUFunctionDefinition
{
public:
    GPUDataType mType;
    HashedString mName;
    std::vector<GPUVariableData> mParameters;
};