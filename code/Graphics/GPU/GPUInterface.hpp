#pragma once

#include "glad/glad.h"
#include "Core/Module.hpp"

class Camera;

enum class GPUBufferType
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

    // Attribute
    void enableAttribute(u32 attributeIndex);
    void disableAttribute(u32 attributeIndex);
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
    void setBufferData(GPUBufferType bufferType, u32 bufferId, const T& data)
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
    void drawElements(u32 elementType, u32 indicesCount, u32 instancesCount, bool instanced);

    // Clear
    void setClearColor(const Vector3& color);
    void clearDepth();
    void clearStencil();
    void clear();

    // Viewport
    void setViewport(u32 x, u32 y, u32 width, u32 height);

    // Flags
    void enableFlag(u32 flag);
    void disableFlag(u32 flag);

    // Blend
    void setBlendFunc(u32 sfactor, u32 dfactor);

    // Depth
    void setDepthFunc(u32 depthFunc);

    // Face mode
    void setFaceMode(bool enableCull, u32 cullMode, u32 frontFaceOrientation);

    // Shaders
    void enableProgram(u32 programId);
    void disableProgram(u32 programId);
    u32 compileProgram(const std::string& vertexShaderString, const std::string& fragmentShaderString);
    void bindSharedBuffer(u32 programId, GPUBufferType bufferType, const std::string& bufferName, u32 bindingPoint);
    
    template<class T>
    void bindUniformValue(u32 programId, const std::string& name, const T& value)
    {
        u32 location = glGetUniformLocation(programId, name.c_str());
        if constexpr (std::is_integral_v<T>)
        {
            if constexpr (std::is_unsigned_v<T>)
            {
                glUniform1ui(location, value);
            }
            else
            {
                glUniform1i(location, value);
            }
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            glUniform1f(location, value);
        }
        else if constexpr (std::is_same_v<T, Vector2>)
        {
            glUniform2fv(location, 1, &value.x);
        }
        else if constexpr (std::is_same_v<T, Vector3>)
        {
            glUniform3fv(location, 1, &value.x);
        }
        else if constexpr (std::is_same_v<T, Vector4>)
        {
            glUniform4fv(location, 1, &value.x);
        }
        else if constexpr (std::is_same_v<T, Matrix4>)
        {
            glUniformMatrix4fv(location, 1, GL_FALSE, value.getData());
        }
        else
        {
            LOG_ERROR("Uniform type not supported!");
        }
    }

private:
    void setBufferDataRaw(GPUBufferType bufferType, u32 VBO, u32 typeSize, u32 size, const void* data);
};
