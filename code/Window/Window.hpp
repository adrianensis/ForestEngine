#pragma once

#include "Input/Input.hpp"
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

class IWindow;
class IWindowListener
{
public:
    virtual void onResize(IWindow* window) = 0;
};

class IWindow
{
public:
    void init(int id, const WindowData& windowData);
    void terminate();
    void update();
    virtual void onUpdate() {};

    GLFWwindow* getGlfwWindow() const;
    std::vector<const char*> getRequiredExtensions() const;

    WindowSize getWindowSize() const;
    float getAspectRatio() const;
    bool isClosed() const;
    void swap();

    void pollEvents() const;
    
    void setCursorVisibility(bool visible);

    void waitUntilNotMinimized() const;
    bool isIconified() const;
    void addWindowListener(IWindowListener* windowListener);

protected:
    Input::InputCursorPosition processCursorPosition(double x, double y) const;
    
    static void onResizeGLFW(GLFWwindow *windowGLFW, int width, int height);
    static void keyCallbackGLFW(GLFWwindow *windowGLFW, int key, int scancode, int action, int mods);
    static void mouseButtonCallbackGLFW(GLFWwindow *windowGLFW, int button, int action, int mods);
    static void scrollCallbackGLFW(GLFWwindow *windowGLFW, double xoffset, double yoffset);
    static void charCallbackGLFW(GLFWwindow *windowGLFW, unsigned int codepoint);
    static void cursorPositionCallbackGLFW(GLFWwindow *windowGLFW, double x, double y);
    
    virtual void onResizeCallback(int width, int height);
    virtual void onKeyCallback(int key, int scancode, int action, int mods) {};
    virtual void onMouseButtonCallback(int button, int action, int mods) {};
    virtual void onScrollCallback(double xoffset, double yoffset) {};
    virtual void onCharCallback(unsigned int codepoint) {};
    virtual void onCursorPositionCallback(double x, double y) {};

protected:
	GLFWwindow *mGLFWWindow = nullptr;
	WindowData mWindowData;
    int mID = -1;

    std::vector<IWindowListener*> mWindowListeners;

public:
    const GLFWwindow* getGLFWWindow() const { return mGLFWWindow; };
    const WindowData& getWindowData() const { return mWindowData; };
};
};
