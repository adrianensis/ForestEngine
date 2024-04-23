#pragma once

#include "Engine/System/System.hpp"
#include "Engine/Input/Input.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

class Window : public System, public IWindowInputAdapter
{
public:
    Vector2 getWindowSize();
    f32 getAspectRatio();
    virtual void init() override;
    virtual void terminate() override;
    bool isClosed();
    void swap();

    void pollEvents() const;
    
    void setCursorVisibility(bool visible);
    virtual Vector2 getMousePosition() const override;

private:
    void onResize(GLFWwindow *window, int width, int height);

    static void onResizeGLFW(GLFWwindow *window, int width, int height);
    static void keyCallbackGLFW(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void mouseButtonCallbackGLFW(GLFWwindow *window, int button, int action, int mods);
    static void scrollCallbackGLFW(GLFWwindow *window, double xoffset, double yoffset);
    static void charCallbackGLFW(GLFWwindow *window, unsigned int codepoint);

    virtual void keyCallback(int key, int scancode, int action, int mods) override;
    virtual void mouseButtonCallback(int button, int action, int mods) override;
    virtual void scrollCallback(double xoffset, double yoffset) override;
    virtual void charCallback(unsigned int codepoint) override;

private:
	GLFWwindow *mGLTFWindow = nullptr;
	Vector2 mWindowSize;

public:
    CGET(GLTFWindow);
    CRGET(WindowSize);
};
REGISTER_CLASS(Window);
