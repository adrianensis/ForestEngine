#pragma once

#include "Core/Event/Event.hpp"
#include "Input/Input.hpp"
#include "GPU/Window/GPUWindow.hpp"
#include "Window/WindowFramework.hpp"

namespace Window
{

class WindowSize
{
public:
    float x = 0;
    float y = 0;
};

class WindowData
{
public:
    WindowSize mWindowSize;
    bool mFullScreen = false;
    bool mMainWindow = false;
    std::string mTitle;
};

class Window;
class IWindowListener
{
public:
    virtual void onResize(Window* window) = 0;
};

class Window: public Input::IWindowInputAdapter, public Event::IEventObject, public IGPUWindow
{
public:
    void init(int id, const WindowData& windowData);
    void terminate();

    GLFWwindow* getGlfwWindow() const;
    std::vector<const char*> getRequiredExtensions() const;

    WindowSize getWindowSize() const;
    float getAspectRatio() const;
    bool isClosed() const;
    void swap();

    void pollEvents() const;
    
    void setCursorVisibility(bool visible);
    virtual Input::InputCursorPosition getMousePosition() const override;

    void waitUntilNotMinimized() const;
    bool isIconified() const;
    void addWindowListener(IWindowListener* windowListener);

    virtual std::vector<const char*> getRequiredGPUExtensions() const override
    { return getRequiredExtensions(); }
    virtual GPUWindowSize getGPUWindowSize() const override
    { return GPUWindowSize{mWindowData.mWindowSize.x, mWindowData.mWindowSize.y}; }
    virtual VkSurfaceKHR createSurface(GPUContext* gpuContext) const override;

private:
    Input::InputCursorPosition processCursorPosition(double x, double y) const;
    void onResize(GLFWwindow *windowGLFW, int width, int height);

    static void onResizeGLFW(GLFWwindow *windowGLFW, int width, int height);
    static void keyCallbackGLFW(GLFWwindow *windowGLFW, int key, int scancode, int action, int mods);
    static void mouseButtonCallbackGLFW(GLFWwindow *windowGLFW, int button, int action, int mods);
    static void scrollCallbackGLFW(GLFWwindow *windowGLFW, double xoffset, double yoffset);
    static void charCallbackGLFW(GLFWwindow *windowGLFW, unsigned int codepoint);
    static void cursorPositionCallbackGLFW(GLFWwindow *windowGLFW, double x, double y);

    virtual void keyCallback(int key, int scancode, int action, int mods) override;
    virtual void mouseButtonCallback(int button, int action, int mods) override;
    virtual void scrollCallback(double xoffset, double yoffset) override;
    virtual void charCallback(unsigned int codepoint) override;
    virtual void cursorPositionCallback(double x, double y) override;

private:
	GLFWwindow *mGLTFWindow = nullptr;
	WindowData mWindowData;
    int mID = -1;

    std::vector<IWindowListener*> mWindowListeners;

public:
    CGET(GLTFWindow);
    CRGET(WindowData);
};
};
