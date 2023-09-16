#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Core/Module.hpp"

class Camera;

class RenderContext : public EngineSystem
{

public:
	friend class Input;
    
    Vector2 getWindowSize();
    f32 getAspectRatio();
    void init();
    bool isClosed();
    void clearDepth();
    void clearStencil();
    void clear();
    void swap();
    void terminate();

private:
    void onResize(GLFWwindow *window, int width, int height);
    static void GLFWonResize(GLFWwindow *window, int width, int height);
    void setDataVBOAnyTypeRaw(u32 VBO, u32 typeSize, u32 size, const void* data);
    void setDataUBOAnyTypeRaw(u32 UBO, u32 typeSize, u32 size, const void* data);
	
private:
	GLFWwindow *smWindow = nullptr;
	Vector2 smWindowSize;
	Camera *mCamera = nullptr;
};
