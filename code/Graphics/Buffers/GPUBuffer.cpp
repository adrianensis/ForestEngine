#include "Graphics/Buffers/GPUBuffer.hpp"

GPUBufferBase::~GPUBufferBase() 
{
	terminate();
}

void GPUBufferBase::init(u32 typeSizeInBytes, bool isStatic)
{
	mTypeSizeInBytes = typeSizeInBytes;
	mIsStatic = isStatic;

	mVBO = RenderContext::createVBO();
}

void GPUBufferBase::terminate()
{
    RenderContext::deleteVBO(mVBO);
}

void GPUBufferBase::resize(u32 size)
{
	RenderContext::resizeVBOAnyType(mVBO, mTypeSizeInBytes, size, mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}

void GPUBufferBase::attribute(u32 attributeIndex, GPUBufferPrimitiveType primitiveType, u32 pointerOffset, u32 divisor)
{
	u32 primitiveTypeSize = 0;
	switch (primitiveType)
	{
		case GPUBufferPrimitiveType::FLOAT:
			primitiveTypeSize = sizeof(f32);
		break;
		case GPUBufferPrimitiveType::INT:
			primitiveTypeSize = sizeof(u32);
		break;
	}

	attributeCustomSize(attributeIndex, primitiveType, mTypeSizeInBytes/primitiveTypeSize, pointerOffset, divisor);
}

void GPUBufferBase::attributeCustomSize(u32 attributeIndex, GPUBufferPrimitiveType primitiveType, u32 elementSize, u32 pointerOffset, u32 divisor)
{
	RenderContext::attribute(attributeIndex, elementSize, (u32)primitiveType, mTypeSizeInBytes, pointerOffset, divisor);
}