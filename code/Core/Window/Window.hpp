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
    void init(Core::i32 id, const WindowData& windowData);
    void terminate();

    GLFWwindow* getGlfwWindow() const;
    std::vector<const char*> getRequiredExtensions() const;

    Vector2 getWindowSize() const;
    Core::f32 getAspectRatio() const;
    bool isClosed() const;
    void swap();

    void pollEvents() const;
    
    void setCursorVisibility(bool visible);
    virtual Vector2 getMousePosition() const override;

    void waitUntilNotMinimized() const;
    bool isIconified() const;
    void addWindowListener(Ptr<IWindowListener> windowListener);

private:
    void onResize(GLFWwindow *window, Core::i32 width, Core::i32 height);

    static void onResizeGLFW(GLFWwindow *window, Core::i32 width, Core::i32 height);
    static void keyCallbackGLFW(GLFWwindow *window, Core::i32 key, Core::i32 scancode, Core::i32 action, Core::i32 mods);
    static void mouseButtonCallbackGLFW(GLFWwindow *window, Core::i32 button, Core::i32 action, Core::i32 mods);
    static void scrollCallbackGLFW(GLFWwindow *window, Core::f64 xoffset, Core::f64 yoffset);
    static void charCallbackGLFW(GLFWwindow *window, Core::u32 codepoint);

    virtual void keyCallback(Core::i32 key, Core::i32 scancode, Core::i32 action, Core::i32 mods) override;
    virtual void mouseButtonCallback(Core::i32 button, Core::i32 action, Core::i32 mods) override;
    virtual void scrollCallback(Core::f64 xoffset, Core::f64 yoffset) override;
    virtual void charCallback(Core::u32 codepoint) override;

private:
	GLFWwindow *mGLTFWindow = nullptr;
	WindowData mWindowData;
    Core::i32 mID = -1;

    std::vector<Ptr<IWindowListener>> mWindowListeners;

public:
    CGET(GLTFWindow);
    CRGET(WindowData);
};
REGISTER_CLASS(Window);
NS_END
