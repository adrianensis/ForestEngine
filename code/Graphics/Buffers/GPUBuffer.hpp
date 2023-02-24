#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"

enum class GPUBufferPrimitiveType : u32
{
    INT = GL_INT,
    FLOAT = GL_FLOAT,
};

class GPUBufferData
{
public:
	GPUBufferData() = default;
    GPUBufferData(u32 typeSizeInBytes): mTypeSizeInBytes(typeSizeInBytes) {};

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
        }

        return primitiveTypeSize;
    }

    u32 getSizePrimitiveType()
    {
        u32 primitiveTypeSizeInBytes = getPrimitiveTypeSizeInBytes();
        u32 sizeInPrimitiveTypes = mTypeSizeInBytes/primitiveTypeSizeInBytes;
        return sizeInPrimitiveTypes;
    }
    
public:
    GPUBufferPrimitiveType mPrimitiveType = GPUBufferPrimitiveType::FLOAT;
    u32 mTypeSizeInBytes = 0;
    u32 mInstanceDivisor = 0;
    u32 mAttributeDivisorSizeInPrimitiveTypes = 0;
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

private:
	u32 mVBO = 0; // TODO: change u32 for GLuint
    GPUBufferData mData;
    u32 mAttributeLocation = 0;
    u32 mAttributeOffset = 0;
    u32 mPreviousOffsetInBytes = 0;
    bool mIsStatic = false;
};