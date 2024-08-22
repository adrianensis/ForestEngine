#pragma once

#include "Core/Input/Input.hpp"
#define GLFW_INCLUDE_NONE
#include "Graphics/GPU/GPUInterface.hpp"
#include "GLFW/glfw3.h"

class GPUWindowData
{
public:
    Vector2 mWindowSize;
    bool mFullScreen = false;
    bool mMainWindow = false;
    HashedString mTitle;
};

class GPUWindow: public ObjectBase, public IWindowInputAdapter
{
public:
    void init(i32 id, const GPUWindowData& gpuWindowData);
    void terminate();

    GLFWwindow* getGlfwWindow() const;
    Vector2 getSizeInPixels() const;

    Vector2 getWindowSize();
    f32 getAspectRatio();
    bool isClosed();
    void swap();

    void pollEvents() const;
    
    void setCursorVisibility(bool visible);
    virtual Vector2 getMousePosition() const override;

private:
    void onResize(GLFWwindow *gpuWindow, i32 width, i32 height);

    static void onResizeGLFW(GLFWwindow *gpuWindow, i32 width, i32 height);
    static void keyCallbackGLFW(GLFWwindow *gpuWindow, i32 key, i32 scancode, i32 action, i32 mods);
    static void mouseButtonCallbackGLFW(GLFWwindow *gpuWindow, i32 button, i32 action, i32 mods);
    static void scrollCallbackGLFW(GLFWwindow *gpuWindow, f64 xoffset, f64 yoffset);
    static void charCallbackGLFW(GLFWwindow *gpuWindow, u32 codepoint);

    virtual void keyCallback(i32 key, i32 scancode, i32 action, i32 mods) override;
    virtual void mouseButtonCallback(i32 button, i32 action, i32 mods) override;
    virtual void scrollCallback(f64 xoffset, f64 yoffset) override;
    virtual void charCallback(u32 codepoint) override;

private:
	GLFWwindow *mGLTFWindow = nullptr;
	GPUWindowData mGPUWindowData;
    i32 mID = -1;

public:
    CGET(GLTFWindow);
    CRGET(GPUWindowData);
};
REGISTER_CLASS(GPUWindow);
