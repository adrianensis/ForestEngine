#pragma once

#include "Core/System/System.hpp"
#include "Core/Window/Window.hpp"
NS_BEGIN(Core)
class WindowManager : public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();

    WeakPtr<Window> createWindow(const WindowData& windowData);
    WeakPtr<Window> getWindow(u32 index) const;
private:
	std::vector<OwnerPtr<Window>> mWindows;
    WeakPtr<Window> mMainWindow;
public:
    GET(MainWindow)
};
REGISTER_CLASS(WindowManager);
NS_END