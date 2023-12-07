#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"

#include <stddef.h>

GLuint GPUInterface::createBuffer()
{
	u32 bufferId;
	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	return bufferId;
}

void GPUInterface::attribute(u32 propertyArrayIndex, u32 elementSize, u32 primitiveType, u32 strideSize, u32 pointerOffset, u32 divisor)
{
	enableProperty(propertyArrayIndex);
	if(primitiveType == GL_INT)
	{
		glVertexAttribIPointer(propertyArrayIndex, elementSize, primitiveType, strideSize, reinterpret_cast<byte*>(pointerOffset));
	}
	else
	{
		glVertexAttribPointer(propertyArrayIndex, elementSize, primitiveType, GL_FALSE, strideSize, reinterpret_cast<byte*>(pointerOffset));
	}

    glVertexAttribDivisor(propertyArrayIndex, divisor);
}

GLuint GPUInterface::createVertexBufferLayout()
{
	u32 vertexBufferLayout;
	glGenVertexArrays(1, &vertexBufferLayout);
	enableVertexBufferLayout(vertexBufferLayout);
	return vertexBufferLayout;
}

u32 GPUInterface::getMaxElementsInSharedBuffer(GPUBufferType bufferType, u32 elementSizeInBytes)
{
    i32 maxBlockSize;
    switch (bufferType)
    {
    case UNIFORM:
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxBlockSize);
        break;
    case STORAGE:
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxBlockSize);
        break;
    default:
        CHECK_MSG(false, "Ilegal GPUBufferType!");
        break;
    }

    i32 maxElements = (maxBlockSize/elementSizeInBytes);
    return maxElements;
}

u32 GPUInterface::getMaxBindingPointsForSharedBuffer(GPUBufferType bufferType)
{
    i32 maxBindingPoints;
    switch (bufferType)
    {
    case UNIFORM:
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBindingPoints);
        break;
    case STORAGE:
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxBindingPoints);
        break;
    default:
        CHECK_MSG(false, "Ilegal GPUBufferType!");
        break;
    }

    return maxBindingPoints;
}

void GPUInterface::bindSharedBuffer(GPUBufferType bufferType, u32 bufferId, u32 bindingPoint)
{
    // define the range of the buffer that links to a uniform binding point
    glBindBufferBase(bufferType, bindingPoint, bufferId);
}

void GPUInterface::resizeBuffer(GPUBufferType bufferType, u32 bufferId, u32 typeSizeInBytes, u32 size, u32 drawMode /*= GL_DYNAMIC_DRAW*/)
{
	glBindBuffer(bufferType, bufferId);
	glBufferData(bufferType, typeSizeInBytes * size, nullptr, drawMode);
}

void GPUInterface::setBufferDataRaw(GPUBufferType bufferType, u32 bufferId, u32 typeSize, u32 size, const void* data)
{
	glBindBuffer(bufferType, bufferId);
	glBufferSubData(bufferType, 0, typeSize * size, data);
}

void GPUInterface::deleteVertexBufferLayout(u32 vertexBufferLayout)
{
    glDeleteVertexArrays(1, &vertexBufferLayout);
}

void GPUInterface::deleteBuffer(u32 bufferId)
{
    glDeleteBuffers(1, &bufferId);
}

void GPUInterface::enableProperty(u32 propertyArrayIndex)
{
	glEnableVertexAttribArray(propertyArrayIndex);
}

void GPUInterface::disableProperty(u32 propertyArrayIndex)
{
	glDisableVertexAttribArray(propertyArrayIndex);
}

void GPUInterface::enableVertexBufferLayout(u32 vertexBufferLayout)
{
	glBindVertexArray(vertexBufferLayout);
}

void GPUInterface::enableStencil(u32 stencilValue, u32 stencilFunction, u32 stencilPassOp)
{
    glEnable(GL_STENCIL_TEST);

    // if(isMask)
    // {
    //     // NOTE: Required stencilValue starts at 1!
    //     glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    //     glStencilFunc(GL_EQUAL, stencilValue-1, 0xFF);

    //     // glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    //     // glDepthMask(GL_FALSE);
    // }
    // else
    // {
    //     glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    //     // Make it so the stencil test only passes when not equal to ref value
    //     glStencilFunc(stencilFunction, stencilValue-1, 0xFF);
    // }

    glStencilOp(GL_KEEP, GL_KEEP, stencilPassOp);
    glStencilFunc(stencilFunction, stencilValue, 0xFF);

}

void GPUInterface::disableStencil()
{
	// glStencilFunc(GL_ALWAYS, 0, 0xFF);
	// glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	// glDepthMask(GL_TRUE);

	glDisable(GL_STENCIL_TEST);
}

GLuint GPUInterface::createTexture(u32 width, u32 height, u32 format, const byte* data, bool createMipMap)
{
    u32 textureId;
    glGenTextures(1, &textureId);

    enableTexture(textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if(createMipMap)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    if(createMipMap)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    disableTexture();

    return textureId;
}

GLuint GPUInterface::createTextureFont(u32 width, u32 height, u32 format, const byte* data)
{
    u32 textureId;
    glGenTextures(1, &textureId);

    enableTexture(textureId);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    disableTexture();

    return textureId;
}

void GPUInterface::subTexture(u32 x, u32 y, u32 width, u32 height, u32 format, const byte* data)
{
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y , width, height, format, GL_UNSIGNED_BYTE, data);
}

void GPUInterface::deleteTexture(u32 textureId)
{
	glDeleteTextures(1, &textureId);
}

void GPUInterface::enableTexture(u32 textureId)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void GPUInterface::disableTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GPUInterface::drawElements(u32 indicesCount, u32 instancesCount, bool instanced)
{
	if(instanced)
	{
		glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0, instancesCount);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, instancesCount * indicesCount, GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
}

void GPUInterface::drawLines(u32 linesCount)
{
	glDrawElements(GL_LINES, linesCount * 2, GL_UNSIGNED_INT, 0);
}

void GPUInterface::clearDepth()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void GPUInterface::clearStencil()
{
	glClear(GL_STENCIL_BUFFER_BIT);
}

void GPUInterface::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
