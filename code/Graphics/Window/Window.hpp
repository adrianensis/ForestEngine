#pragma once

#include "Engine/Input/Input.hpp"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

class WindowData
{
public:
    Vector2 mWindowSize;
    bool mFullScreen = false;
    HashedString mTitle;
};

class Window: public IWindowInputAdapter
{
public:
    void init(i32 id, const WindowData& windowData);
    void terminate();

    Vector2 getWindowSize();
    f32 getAspectRatio();
    bool isClosed();
    void swap();

    void pollEvents() const;
    
    void setCursorVisibility(bool visible);
    virtual Vector2 getMousePosition() const override;

private:
    void onResize(GLFWwindow *window, i32 width, i32 height);

    static void onResizeGLFW(GLFWwindow *window, i32 width, i32 height);
    static void keyCallbackGLFW(GLFWwindow *window, i32 key, i32 scancode, i32 action, i32 mods);
    static void mouseButtonCallbackGLFW(GLFWwindow *window, i32 button, i32 action, i32 mods);
    static void scrollCallbackGLFW(GLFWwindow *window, f64 xoffset, f64 yoffset);
    static void charCallbackGLFW(GLFWwindow *window, u32 codepoint);

    virtual void keyCallback(i32 key, i32 scancode, i32 action, i32 mods) override;
    virtual void mouseButtonCallback(i32 button, i32 action, i32 mods) override;
    virtual void scrollCallback(f64 xoffset, f64 yoffset) override;
    virtual void charCallback(u32 codepoint) override;

private:
	GLFWwindow *mGLTFWindow = nullptr;
	WindowData mWindowData;
    i32 mID = -1;

public:
    CGET(GLTFWindow);
    CRGET(WindowData);
};
REGISTER_CLASS(Window);
