#pragma once

#include "glad/glad.h"
#include "Core/Module.hpp"

class Camera;

class GPUInterface : public EngineSystem
{
public:
    void clearDepth();
    void clearStencil();
    void clear();
    GLuint createVBO();
    void attribute(u32 propertyArrayIndex, u32 elementSize, u32 primitiveType, u32 strideSize, u32 pointerOffset, u32 divisor);
    GLuint createVAO();
    GLuint createEBO();
    GLuint createUBO();
    void resizeVBO(u32 VBO, u32 size, u32 drawMode = GL_DYNAMIC_DRAW);
    void resizeVBOU32(u32 VBO, u32 size, u32 drawMode = GL_DYNAMIC_DRAW);
    void resizeVBOAnyType(u32 VBO, u32 typeSizeInBytes, u32 size, u32 drawMode = GL_DYNAMIC_DRAW);
    void resizeEBO(u32 EBO, u32 size, u32 drawMode = GL_DYNAMIC_DRAW);
    void resizeUBOAnyType(u32 UBO, u32 typeSizeInBytes, u32 size, u32 drawMode = GL_DYNAMIC_DRAW);
    
    void setDataVBO(u32 VBO, const std::vector<f32>& data);
    void setDataVBOU32(u32 VBO, const std::vector<u32>& data);
    template<class T>
    void setDataVBOAnyType(u32 VBO, const std::vector<T>& data)
    {
        setDataVBOAnyTypeRaw(VBO, sizeof(T), data.size(), data.data());
    }
    void setDataEBO(u32 EBO, const std::vector<Face>& data);
    void setDataEBORaw(u32 EBO, const std::vector<u32>& data);
    template<class T>
    void setDataUBOAnyType(u32 UBO, const std::vector<T>& data)
    {
        setDataUBOAnyTypeRaw(UBO, sizeof(T), data.size(), data.data());
    }

    void deleteVAO(u32 VAO);
    void deleteVBO(u32 VBO);
    void deleteEBO(u32 EBO);
    void deleteUBO(u32 UBO);

    void enableProperty(u32 propertyArrayIndex);
    void disableProperty(u32 propertyArrayIndex);

    void enableVAO(u32 VAO);
    
    void enableStencil(u32 stencilValue, u32 stencilFunction, u32 stencilPassOp);
    void disableStencil();

    GLuint createTexture(u32 width, u32 height, const byte* data, bool createMipMap);
    void deleteTexture(u32 textureId);
    void enableTexture(u32 textureId);
    void disableTexture();

    void drawElements(u32 indicesCount, u32 instancesCount, bool instanced);
    void drawLines(u32 linesCount);

private:
    void setDataVBOAnyTypeRaw(u32 VBO, u32 typeSize, u32 size, const void* data);
    void setDataUBOAnyTypeRaw(u32 UBO, u32 typeSize, u32 size, const void* data);
};