#pragma once

#include "Engine/Core.hpp"
#include "Core/Event/EventsManager.hpp"
#include "Window/Window.hpp"
#include "GPU/Window/GPUWindow.hpp"
#include "Window/WindowManager.hpp"

class EngineInput: public Input::IInput
{

};

class EngineWindow : public Window::IWindow, public Input::IWindowInputAdapter, public Event::IEventObject, public IGPUWindow
{
public:
    virtual std::vector<const char*> getRequiredGPUExtensions() const override
    { return getRequiredExtensions(); }
    virtual GPUWindowSize getGPUWindowSize() const override
    { return GPUWindowSize{mWindowData.mWindowSize.x, mWindowData.mWindowSize.y}; }
    virtual VkSurfaceKHR createSurface(GPUContext* gpuContext) const override;

    virtual void onUpdate() override;

    virtual void onKeyCallback(int key, int scancode, int action, int mods) override;
    virtual void onMouseButtonCallback(int button, int action, int mods) override;
    virtual void onScrollCallback(double xoffset, double yoffset) override;
    virtual void onCharCallback(unsigned int codepoint) override;
    virtual void onCursorPositionCallback(double x, double y) override;
};

class Engine
{

public:
    void init();
    void preSceneChanged();
    void postSceneChanged();
    void run();
    void terminate();

private:
	Core::f32 mFPS = 0.0f;
};