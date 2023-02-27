#pragma once

#include "Core/Module.hpp"

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
    GPUDataType mGPUDataType;
    std::string mName;
    std::string mValue;
    std::string mArraySize;
};

class GPUBufferData
{
public:
	GPUBufferData() = default;
    GPUBufferData(const GPUDataType& gpuDataType, const std::string_view& attributeName)
    {
        mGPUVariableData.mName = attributeName;
        mGPUVariableData.mGPUDataType = gpuDataType;
    };
    GPUBufferData(const GPUVariableData& gpuVariableData): mGPUVariableData(gpuVariableData) {};
    
public:
    u32 mInstanceDivisor = 0;
    u32 mAttributeDivisorSizeInPrimitiveTypes = 0;
    GPUVariableData mGPUVariableData;
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