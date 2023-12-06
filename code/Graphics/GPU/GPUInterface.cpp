#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"

#include <stddef.h>

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

GLuint GPUInterface::createBuffer()
{
	u32 VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	return VBO;
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

GLuint GPUInterface::createVAO()
{
	u32 VAO;
	glGenVertexArrays(1, &VAO);
	enableVAO(VAO);
	return VAO;
}

u32 GPUInterface::getMaxElementsInUBO(u32 elementSizeInBytes)
{
    i32 gl_MAX_UNIFORM_BLOCK_SIZE;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &gl_MAX_UNIFORM_BLOCK_SIZE);
    // vectors / 4 components = matrices
    i32 gl_MAX_UNIFORM_BLOCK_SIZE_IN_ELEMENTS = (gl_MAX_UNIFORM_BLOCK_SIZE/elementSizeInBytes);
    LOG_VAR(gl_MAX_UNIFORM_BLOCK_SIZE_IN_ELEMENTS);
    return gl_MAX_UNIFORM_BLOCK_SIZE_IN_ELEMENTS;
}

void GPUInterface::bindUBO(u32 UBO, u32 bindingPoint)
{
    // define the range of the buffer that links to a uniform binding point
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, UBO);
}

void GPUInterface::bindSSBO(u32 SSBO, u32 bindingPoint)
{
    // define the range of the buffer that links to a uniform binding point
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, SSBO);
}

void GPUInterface::resizeVBO(u32 VBO, u32 size, u32 drawMode /*= GL_DYNAMIC_DRAW*/)
{
	resizeVBOAnyType(VBO, sizeof(f32), size,drawMode);
}

void GPUInterface::resizeVBOU32(u32 VBO, u32 size, u32 drawMode /*= GL_DYNAMIC_DRAW*/)
{
	resizeVBOAnyType(VBO, sizeof(u32), size,drawMode);
}

void GPUInterface::resizeVBOAnyType(u32 VBO, u32 typeSizeInBytes, u32 size, u32 drawMode /*= GL_DYNAMIC_DRAW*/)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, typeSizeInBytes * size, nullptr, drawMode);
}

void GPUInterface::resizeEBO(u32 EBO, u32 size, u32 drawMode /*= GL_DYNAMIC_DRAW*/)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * size, nullptr, drawMode);
}

void GPUInterface::resizeUBOAnyType(u32 UBO, u32 typeSizeInBytes, u32 size, u32 drawMode /*= GL_DYNAMIC_DRAW*/)
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, typeSizeInBytes * size, nullptr, drawMode);
}

void GPUInterface::resizeSSBOAnyType(u32 SSBO, u32 typeSizeInBytes, u32 size, u32 drawMode /*= GL_DYNAMIC_DRAW*/)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, typeSizeInBytes * size, nullptr, drawMode);
}

void GPUInterface::setDataVBO(u32 VBO, const std::vector<f32>& data)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(f32) * data.size(), data.data());
}

void GPUInterface::setDataVBOU32(u32 VBO, const std::vector<u32>& data)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(u32) * data.size(), data.data());
}

void GPUInterface::setDataVBOAnyTypeRaw(u32 VBO, u32 typeSize, u32 size, const void* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, typeSize * size, data);
}

void GPUInterface::setDataUBOAnyTypeRaw(u32 UBO, u32 typeSize, u32 size, const void* data)
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, typeSize * size, data);
}

void GPUInterface::setDataSSBOAnyTypeRaw(u32 SSBO, u32 typeSize, u32 size, const void* data)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, typeSize * size, data);
}

void GPUInterface::setDataEBO(u32 EBO, const std::vector<Face>& data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(Face) * data.size(), data.data());
}

void GPUInterface::setDataEBORaw(u32 EBO, const std::vector<u32>& data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(u32) * data.size(), data.data());
}

void GPUInterface::deleteVAO(u32 VAO)
{
    glDeleteVertexArrays(1, &VAO);
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

void GPUInterface::enableVAO(u32 VAO)
{
	glBindVertexArray(VAO);
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