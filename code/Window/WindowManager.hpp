#pragma once

#include "Core/System/System.hpp"
#include "Window/Window.hpp"
NS_BEGIN(Window)
class WindowManager : public Core::System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();

    Core::WeakPtr<Window> createWindow(const WindowData& windowData);
    Core::WeakPtr<Window> getWindow(Core::u32 index) const;
private:
	std::vector<Core::OwnerPtr<Window>> mWindows;
    Core::WeakPtr<Window> mMainWindow;
public:
    GET(MainWindow)
};
REGISTER_CLASS(WindowManager);
NS_END