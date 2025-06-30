#pragma once

#include "Core/Input/Input.hpp"
#include "Core/Window/WindowFramework.hpp"

NS_BEGIN(Core)

class WindowData
{
public:
    Vector2 mWindowSize;
    bool mFullScreen = false;
    bool mMainWindow = false;
    HashedString mTitle;
};

class IWindowListener
{
public:
    virtual void onResize() = 0;
};

class Window: public IWindowInputAdapter
{
public:
    void init(i32 id, const WindowData& windowData);
    void terminate();

    GLFWwindow* getGlfwWindow() const;
    std::vector<const char*> getRequiredExtensions() const;

    Vector2 getWindowSize() const;
    f32 getAspectRatio() const;
    bool isClosed() const;
    void swap();

    void pollEvents() const;
    
    void setCursorVisibility(bool visible);
    virtual Vector2 getMousePosition() const override;

    void waitUntilNotMinimized() const;
    bool isIconified() const;
    void addWindowListener(Ptr<IWindowListener> windowListener);

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

    std::vector<Ptr<IWindowListener>> mWindowListeners;

public:
    CGET(GLTFWindow);
    CRGET(WindowData);
};
REGISTER_CLASS(Window);
NS_END
