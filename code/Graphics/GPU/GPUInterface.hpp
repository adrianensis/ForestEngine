#pragma once

#include "glad/glad.h"
#include "Core/Module.hpp"

class Camera;

enum GPUBufferType
{
    VERTEX = GL_ARRAY_BUFFER,
    INDEX = GL_ELEMENT_ARRAY_BUFFER,
    UNIFORM = GL_UNIFORM_BUFFER,
    STORAGE = GL_SHADER_STORAGE_BUFFER
};

class GPUInterface : public System
{
public:
    // Vertex buffers layout
    GLuint createVertexBufferLayout();
    void enableVertexBufferLayout(u32 vertexBufferLayout);
    void deleteVertexBufferLayout(u32 vertexBufferLayout);

    // Property / Attribute
    void enableProperty(u32 propertyArrayIndex);
    void disableProperty(u32 propertyArrayIndex);
    void attribute(u32 propertyArrayIndex, u32 elementSize, u32 primitiveType, u32 strideSize, u32 pointerOffset, u32 divisor);

    // Buffer
    GLuint createBuffer();
    void resizeBuffer(GPUBufferType bufferType, u32 bufferId, u32 typeSizeInBytes, u32 size, u32 drawMode = GL_DYNAMIC_DRAW);

    template<class T>
    void setBufferDataArray(GPUBufferType bufferType, u32 bufferId, const std::vector<T>& data)
    {
        setBufferDataRaw(bufferType, bufferId, sizeof(T), data.size(), data.data());
    }

    template<class T>
    void setBufferDataStruct(GPUBufferType bufferType, u32 bufferId, const T& data)
    {
        setBufferDataRaw(bufferType, bufferId, sizeof(T), 1, &data);
    }

    void deleteBuffer(u32 bufferId);

    // Shared Buffer
    u32 getMaxElementsInSharedBuffer(GPUBufferType bufferType, u32 elementSizeInBytes);
    u32 getMaxBindingPointsForSharedBuffer(GPUBufferType bufferType);
    void bindSharedBuffer(GPUBufferType bufferType, u32 bufferId, u32 bindingPoint);
    
    // Stencil
    void enableStencil(u32 stencilValue, u32 stencilFunction, u32 stencilPassOp);
    void disableStencil();

    // Texture
    GLuint createTexture(u32 width, u32 height, u32 format, const byte* data, bool createMipMap);
    GLuint createTextureFont(u32 width, u32 height, u32 format, const byte* data);
    void subTexture(u32 x, u32 y, u32 width, u32 height, u32 format,  const byte* data);
    void deleteTexture(u32 textureId);
    void enableTexture(u32 textureId);
    void disableTexture();

    // Draw Call
    void drawElements(u32 indicesCount, u32 instancesCount, bool instanced);
    void drawLines(u32 linesCount);

    // Clear
    void clearDepth();
    void clearStencil();
    void clear();

private:
    void setBufferDataRaw(GPUBufferType bufferType, u32 VBO, u32 typeSize, u32 size, const void* data);
};
