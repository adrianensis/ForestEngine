#pragma once

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

class Window: public Input::IWindowInputAdapter, public IGPUWindow
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
	WindowData mWindowData;
    int mID = -1;

    std::vector<IWindowListener*> mWindowListeners;

public:
    CGET(GLTFWindow);
    CRGET(WindowData);
};
};
