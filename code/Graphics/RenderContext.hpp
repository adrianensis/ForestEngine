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
    static GLuint createVBO(u32 elementSize, u32 PropertyArrayIndex);
    static GLuint createVAO();
    static GLuint createEBO();
    static void resizeVBO(u32 VBO, u32 size, u32 drawMode = GL_DYNAMIC_DRAW);
    static void resizeEBO(u32 EBO, u32 size, u32 drawMode = GL_DYNAMIC_DRAW);
    static void setDataVBO(u32 VBO, const std::vector<f32> &data);
    static void setDataEBO(u32 EBO, const std::vector<u32> &data);
    static void enableProperty(u32 PropertyArrayIndex);
    static void disableProperty(u32 PropertyArrayIndex);
    static void enableVAO(u32 VAO);
    static void drawElements(u32 indicesCount, u32 instancesCount, bool instanced);
    static void drawLines(u32 linesCount);

private:
    static void onResize(GLFWwindow *window, int width, int height);
	
private:
	inline static GLFWwindow *smWindow = nullptr;
	inline static Vector2 smWindowSize;
	inline static Camera *mCamera = nullptr;
};

#endif