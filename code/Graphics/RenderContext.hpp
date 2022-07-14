#ifndef RENDERCONTEXT_HPP
#define RENDERCONTEXT_HPP

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Core/Module.hpp"


class Camera;

class RenderContext
{

public:
	friend class Input;
    
    static Vector2 getWindowSize();
    static f32 getAspectRatio();
    static void init();
    static bool isClosed();
    static void clear();
    static void swap();
    static void terminate();
    static GLuint createVBO(u32 elementSize, u32 propertyArrayIndex);
    static GLuint createVBOU32(u32 elementSize, u32 propertyArrayIndex);
    static GLuint createVBOAnyType(u32 elementSize,  u32 typeSize, u32 propertyArrayIndex);
    static GLuint createVAO();
    static GLuint createEBO();
    static void resizeVBO(u32 VBO, u32 size, u32 drawMode = GL_DYNAMIC_DRAW);
    static void resizeVBOU32(u32 VBO, u32 size, u32 drawMode = GL_DYNAMIC_DRAW);
    static void resizeVBOAnyType(u32 VBO, u32 typeSize, u32 size, u32 drawMode = GL_DYNAMIC_DRAW);
    static void resizeEBO(u32 EBO, u32 size, u32 drawMode = GL_DYNAMIC_DRAW);
    static void setDataVBO(u32 VBO, CR(std::vector<f32>)data);
    static void setDataVBOU32(u32 VBO, CR(std::vector<u32>)data);
    
    template<class T>
    static void setDataVBOAnyType(u32 VBO, CR(std::vector<T>)data)
    {
        setDataVBOAnyTypeRaw(VBO, sizeof(T), data.size(), data.data());
    }

    static void setDataEBO(u32 EBO, CR(std::vector<u32>)data);
    static void enableProperty(u32 propertyArrayIndex);
    static void disableProperty(u32 propertyArrayIndex);
    static void enableVAO(u32 VAO);
    static void drawElements(u32 indicesCount, u32 instancesCount, bool instanced);
    static void drawLines(u32 linesCount);

private:
    static void onResize(GLFWwindow *window, int width, int height);
    static void setDataVBOAnyTypeRaw(u32 VBO, u32 typeSize, u32 size, const void* data);
	
private:
	inline static GLFWwindow *smWindow = nullptr;
	inline static Vector2 smWindowSize;
	inline static Camera *mCamera = nullptr;
};

#endif